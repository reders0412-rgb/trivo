#include "Renderer.h"
#include "Scene.h"
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <QMatrix3x3>
#include <cstring>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// GLSL Sources
// ─────────────────────────────────────────────────────────────────────────────

static const char *PBR_VERT = R"GLSL(
#version 410 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aUV;
layout(location=3) in vec3 aTangent;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uNormalMat;

out vec3 vPos;
out vec3 vNormal;
out vec2 vUV;
out vec3 vTangent;

void main(){
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vPos    = worldPos.xyz;
    vNormal = normalize(uNormalMat * aNormal);
    vTangent= normalize(uNormalMat * aTangent);
    vUV     = aUV;
    gl_Position = uProj * uView * worldPos;
}
)GLSL";

static const char *PBR_FRAG = R"GLSL(
#version 410 core
in vec3 vPos;
in vec3 vNormal;
in vec2 vUV;
in vec3 vTangent;

out vec4 fragColor;

uniform vec3  uCamPos;
uniform vec3  uBaseColor;
uniform float uRoughness;
uniform float uMetallic;

// Up to 4 lights
uniform int   uLightCount;
uniform vec3  uLightDir[4];
uniform vec3  uLightColor[4];
uniform float uLightIntensity[4];
uniform int   uLightType[4];  // 0=dir 1=point

uniform vec3  uAmbient;
uniform float uAmbientStr;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N,vec3 H,float r){
    float a=r*r; float a2=a*a;
    float d=max(dot(N,H),0.0); float d2=d*d;
    float denom=d2*(a2-1.0)+1.0;
    return a2/(PI*denom*denom+0.0001);
}
float GeometrySchlick(float d,float r){
    float k=(r+1.0)*(r+1.0)/8.0;
    return d/(d*(1.0-k)+k);
}
float GeometrySmith(vec3 N,vec3 V,vec3 L,float r){
    return GeometrySchlick(max(dot(N,V),0.0),r)*GeometrySchlick(max(dot(N,L),0.0),r);
}
vec3 fresnelSchlick(float c,vec3 F0){ return F0+(1.0-F0)*pow(clamp(1.0-c,0.0,1.0),5.0); }

void main(){
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uCamPos - vPos);
    vec3 albedo = pow(uBaseColor, vec3(2.2)); // linear
    float metal = uMetallic;
    float rough = max(uRoughness, 0.04);

    vec3 F0 = mix(vec3(0.04), albedo, metal);
    vec3 Lo = vec3(0.0);

    for(int i=0;i<uLightCount;i++){
        vec3 L;
        float attn = 1.0;
        if(uLightType[i]==0){
            L = normalize(-uLightDir[i]);
        } else {
            vec3 toL = uLightDir[i] - vPos; // position stored in dir for point
            float d  = length(toL);
            L    = toL/d;
            attn = 1.0/(d*d+0.0001);
        }
        vec3 H = normalize(V+L);
        vec3 rad = uLightColor[i] * uLightIntensity[i] * attn;

        float NDF = DistributionGGX(N,H,rough);
        float G   = GeometrySmith(N,V,L,rough);
        vec3  F   = fresnelSchlick(max(dot(H,V),0.0),F0);

        vec3 num   = NDF*G*F;
        float denom= 4.0*max(dot(N,V),0.0)*max(dot(N,L),0.0)+0.0001;
        vec3 spec  = num/denom;

        vec3 kD = (vec3(1.0)-F)*(1.0-metal);
        float NL= max(dot(N,L),0.0);
        Lo += (kD*albedo/PI + spec)*rad*NL;
    }

    vec3 ambient = uAmbient * uAmbientStr * albedo;
    vec3 color   = ambient + Lo;
    // Tone mapping + gamma
    color = color/(color+vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    fragColor = vec4(color,1.0);
}
)GLSL";

static const char *GRID_VERT = R"GLSL(
#version 410 core
layout(location=0) in vec3 aPos;
uniform mat4 uVP;
out float vDist;
void main(){
    vDist = length(aPos.xz);
    gl_Position = uVP * vec4(aPos,1.0);
}
)GLSL";

static const char *GRID_FRAG = R"GLSL(
#version 410 core
in float vDist;
out vec4 fragColor;
void main(){
    float alpha = 1.0 - smoothstep(30.0, 60.0, vDist);
    fragColor = vec4(0.4, 0.4, 0.5, alpha * 0.35);
}
)GLSL";

// ─────────────────────────────────────────────────────────────────────────────
// Renderer
// ─────────────────────────────────────────────────────────────────────────────

Renderer::Renderer()  = default;
Renderer::~Renderer()
{
    if (m_gridVao) glDeleteVertexArrays(1, &m_gridVao);
    if (m_gridVbo) glDeleteBuffers(1, &m_gridVbo);
}

void Renderer::initialize()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setupShaders();

    // ── Build grid geometry ───────────────────────────────────────────────────
    std::vector<float> gridVerts;
    const float ext = 100.f;
    const int   N   = 40;
    for (int i = -N; i <= N; ++i) {
        float p = i * (ext / N);
        // X line
        gridVerts.insert(gridVerts.end(), {-ext, 0, p,  ext, 0, p});
        // Z line
        gridVerts.insert(gridVerts.end(), {p, 0, -ext,  p, 0, ext});
    }

    glGenVertexArrays(1, &m_gridVao);
    glGenBuffers(1, &m_gridVbo);
    glBindVertexArray(m_gridVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_gridVbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(gridVerts.size()*sizeof(float)),
                 gridVerts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Renderer::setupShaders()
{
    m_pbr = std::make_unique<QOpenGLShaderProgram>();
    m_pbr->addShaderFromSourceCode(QOpenGLShader::Vertex,   PBR_VERT);
    m_pbr->addShaderFromSourceCode(QOpenGLShader::Fragment, PBR_FRAG);
    m_pbr->link();

    m_grid = std::make_unique<QOpenGLShaderProgram>();
    m_grid->addShaderFromSourceCode(QOpenGLShader::Vertex,   GRID_VERT);
    m_grid->addShaderFromSourceCode(QOpenGLShader::Fragment, GRID_FRAG);
    m_grid->link();
}

// ── Render frame ─────────────────────────────────────────────────────────────

void Renderer::render(const std::shared_ptr<Scene> &scene,
                       const QMatrix4x4 &view,
                       const QMatrix4x4 &proj,
                       const QVector3D  &camPos,
                       const LightSystem &lights,
                       double /*animTime*/)
{
    // Draw grid first (transparent)
    m_grid->bind();
    m_grid->setUniformValue("uVP", proj * view);
    glBindVertexArray(m_gridVao);
    // count: (2*N+1)*2 lines, 2 verts each
    glDrawArrays(GL_LINES, 0, (40*2+1)*4);
    glBindVertexArray(0);
    m_grid->release();

    // Draw each model
    m_pbr->bind();
    m_pbr->setUniformValue("uCamPos", camPos);

    // Ambient
    const QColor amb = lights.ambientColor();
    m_pbr->setUniformValue("uAmbient",
        QVector3D(amb.redF(), amb.greenF(), amb.blueF()));
    m_pbr->setUniformValue("uAmbientStr", lights.ambientStrength());

    // Lights
    bindLights(lights);

    for (const auto &model : scene->models()) {
        if (!model->visible) continue;

        // Upload if needed
        if (m_gpuModels.find(model->assimpScene) == m_gpuModels.end()) {
            uploadModel(*model);
        }

        // Model matrix
        QMatrix4x4 modelMat;
        modelMat.translate(model->position);
        modelMat.rotate(model->rotation.x(), 1,0,0);
        modelMat.rotate(model->rotation.y(), 0,1,0);
        modelMat.rotate(model->rotation.z(), 0,0,1);
        modelMat.scale(model->scale);

        QMatrix3x3 normalMat = modelMat.normalMatrix();
        m_pbr->setUniformValue("uModel",     modelMat);
        m_pbr->setUniformValue("uView",      view);
        m_pbr->setUniformValue("uProj",      proj);
        m_pbr->setUniformValue("uNormalMat", normalMat);

        auto it = m_gpuModels.find(model->assimpScene);
        if (it == m_gpuModels.end()) continue;

        for (const auto &mesh : it->second.meshes) {
            m_pbr->setUniformValue("uBaseColor",
                QVector3D(mesh.baseColorR, mesh.baseColorG, mesh.baseColorB));
            m_pbr->setUniformValue("uRoughness", mesh.roughness);
            m_pbr->setUniformValue("uMetallic",  mesh.metallic);

            glBindVertexArray(mesh.vao);
            glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
        }
    }
    glBindVertexArray(0);
    m_pbr->release();
}

void Renderer::bindLights(const LightSystem &lights)
{
    const auto &ls = lights.lights();
    int n = static_cast<int>(std::min(ls.size(), size_t(4)));
    m_pbr->setUniformValue("uLightCount", n);
    for (int i = 0; i < n; ++i) {
        const auto &l = ls[i];
        QString prefix = QString("uLight%1").arg(i);
        // We use array syntax
        QString d = QString("uLightDir[%1]").arg(i);
        QString c = QString("uLightColor[%1]").arg(i);
        QString in= QString("uLightIntensity[%1]").arg(i);
        QString tp= QString("uLightType[%1]").arg(i);
        QVector3D dir = (l.type == Light::Point)
                         ? l.position
                         : l.direction.normalized();
        m_pbr->setUniformValue(d.toLatin1(), dir);
        m_pbr->setUniformValue(c.toLatin1(),
            QVector3D(l.color.redF(), l.color.greenF(), l.color.blueF()));
        m_pbr->setUniformValue(in.toLatin1(), l.intensity);
        m_pbr->setUniformValue(tp.toLatin1(), (int)l.type);
    }
}

// ── GPU upload ────────────────────────────────────────────────────────────────

void Renderer::uploadModel(SceneModel &model)
{
    const aiScene *sc = static_cast<const aiScene*>(model.assimpScene);
    if (!sc) return;

    GpuModel gpu;
    uploadMeshes(gpu, sc, sc->mRootNode);
    m_gpuModels[model.assimpScene] = std::move(gpu);
}

void Renderer::uploadMeshes(GpuModel &gpu, const aiScene *sc, const aiNode *node)
{
    for (unsigned i = 0; i < node->mNumMeshes; ++i) {
        const aiMesh *m = sc->mMeshes[node->mMeshes[i]];
        gpu.meshes.push_back(uploadMesh(m, sc));
    }
    for (unsigned i = 0; i < node->mNumChildren; ++i)
        uploadMeshes(gpu, sc, node->mChildren[i]);
}

GpuMesh Renderer::uploadMesh(const aiMesh *m, const aiScene *sc)
{
    struct Vertex { float x,y,z, nx,ny,nz, u,v, tx,ty,tz; };
    std::vector<Vertex>  verts(m->mNumVertices);
    std::vector<unsigned> idx;

    for (unsigned i = 0; i < m->mNumVertices; ++i) {
        auto &v = verts[i];
        v.x=m->mVertices[i].x; v.y=m->mVertices[i].y; v.z=m->mVertices[i].z;
        if (m->HasNormals()) {
            v.nx=m->mNormals[i].x; v.ny=m->mNormals[i].y; v.nz=m->mNormals[i].z;
        }
        if (m->mTextureCoords[0]) {
            v.u=m->mTextureCoords[0][i].x; v.v=m->mTextureCoords[0][i].y;
        }
        if (m->HasTangentsAndBitangents()) {
            v.tx=m->mTangents[i].x; v.ty=m->mTangents[i].y; v.tz=m->mTangents[i].z;
        }
    }
    for (unsigned i = 0; i < m->mNumFaces; ++i) {
        for (unsigned j = 0; j < m->mFaces[i].mNumIndices; ++j)
            idx.push_back(m->mFaces[i].mIndices[j]);
    }

    GpuMesh gm;
    gm.indexCount = static_cast<int>(idx.size());

    // Material colour
    if (m->mMaterialIndex < sc->mNumMaterials) {
        aiMaterial *mat = sc->mMaterials[m->mMaterialIndex];
        aiColor4D col;
        if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, col) == AI_SUCCESS) {
            gm.baseColorR = col.r; gm.baseColorG = col.g; gm.baseColorB = col.b;
        }
        float rough = 0.5f, metal = 0.f;
        mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, rough);
        mat->Get(AI_MATKEY_METALLIC_FACTOR,  metal);
        gm.roughness = rough; gm.metallic = metal;
    }

    glGenVertexArrays(1, &gm.vao);
    glGenBuffers(1, &gm.vbo);
    glGenBuffers(1, &gm.ebo);

    glBindVertexArray(gm.vao);
    glBindBuffer(GL_ARRAY_BUFFER, gm.vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(verts.size()*sizeof(Vertex)),
                 verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gm.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(idx.size()*sizeof(unsigned)),
                 idx.data(), GL_STATIC_DRAW);

    const GLsizei stride = sizeof(Vertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3*4));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6*4));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8*4));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    return gm;
}

void Renderer::deleteModel(SceneModel &model)
{
    auto it = m_gpuModels.find(model.assimpScene);
    if (it == m_gpuModels.end()) return;
    for (auto &mesh : it->second.meshes) {
        glDeleteVertexArrays(1, &mesh.vao);
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ebo);
    }
    m_gpuModels.erase(it);
}
