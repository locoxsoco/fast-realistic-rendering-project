// Author: Marc Comino 2020

#include <glwidget.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <QBuffer>

#include "./mesh_io.h"
#include "./triangle_mesh.h"

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <QRandomGenerator>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

namespace {

const double kFieldOfView = 60;
const double kZNear = 0.3;
const double kZFar = 2;

const char kPhongVertexShaderFile[] = "../../ViewerPBS/shaders/phong.vert";
const char kPhongFragmentShaderFile[] = "../../ViewerPBS/shaders/phong.frag";
const char kTextureMappingColorVertexShaderFile[] = "../../ViewerPBS/shaders/texture_mapping_color.vert";
const char kTextureMappingColorFragmentShaderFile[] = "../../ViewerPBS/shaders/texture_mapping_color.frag";
const char kTextureMappingMetalnessVertexShaderFile[] = "../../ViewerPBS/shaders/texture_mapping_metalness.vert";
const char kTextureMappingMetalnessFragmentShaderFile[] = "../../ViewerPBS/shaders/texture_mapping_metalness.frag";
const char kTextureMappingRoughnessVertexShaderFile[] = "../../ViewerPBS/shaders/texture_mapping_roughness.vert";
const char kTextureMappingRoughnessFragmentShaderFile[] = "../../ViewerPBS/shaders/texture_mapping_roughness.frag";
const char kReflectionVertexShaderFile[] = "../../ViewerPBS/shaders/reflection.vert";
const char kReflectionFragmentShaderFile[] = "../../ViewerPBS/shaders/reflection.frag";
const char kBRDFVertexShaderFile[] = "../../ViewerPBS/shaders/brdf.vert";
const char kBRDFFragmentShaderFile[] = "../../ViewerPBS/shaders/brdf.frag";
const char kSkyVertexShaderFile[] = "../../ViewerPBS/shaders/sky.vert";
const char kSkyFragmentShaderFile[] = "../../ViewerPBS/shaders/sky.frag";
const char kDiffuseIrradianceVertexShaderFile[] = "../../ViewerPBS/shaders/diffuse_irradiance.vert";
const char kDiffuseIrradianceFragmentShaderFile[] = "../../ViewerPBS/shaders/diffuse_irradiance.frag";
const char kSpecularIrradianceVertexShaderFile[] = "../../ViewerPBS/shaders/specular_irradiance.vert";
const char kSpecularIrradianceFragmentShaderFile[] = "../../ViewerPBS/shaders/specular_irradiance.frag";
const char kStepOneVertexShaderFile[] = "../../ViewerPBS/shaders/step_one.vert";
const char kStepOneFragmentShaderFile[] = "../../ViewerPBS/shaders/step_one.frag";
const char kStepTwoVertexShaderFile[] = "../../ViewerPBS/shaders/step_two.vert";
const char kStepTwoFragmentShaderFile[] = "../../ViewerPBS/shaders/step_two.frag";
const char kStepThreeVertexShaderFile[] = "../../ViewerPBS/shaders/step_three.vert";
const char kStepThreeFragmentShaderFile[] = "../../ViewerPBS/shaders/step_three.frag";
const char kStepFourVertexShaderFile[] = "../../ViewerPBS/shaders/step_four.vert";
const char kStepFourFragmentShaderFile[] = "../../ViewerPBS/shaders/step_four.frag";


const int kVertexAttributeIdx = 0;
const int kNormalAttributeIdx = 1;
const int kTextureAttributeIdx = 2;

// SSAO Kernel
std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
//float x = randomFloats(*QRandomGenerator::global());
std::vector<glm::vec2> ssao_kernel;                              /* use circle of radius 2 */

// light
// ------
glm::vec3 light_position = glm::vec3(-5.f, 5.f, 5.f);
glm::vec3 light_color = glm::vec3(300.0f, 300.0f, 300.0f);
//Silver Material
glm::vec3 material_ambient = glm::vec3(0.19225f, 0.19225f, 0.19225f);
glm::vec3 material_diffuse = glm::vec3(0.50754f, 0.50754f, 0.50754f);
glm::vec3 material_specular = glm::vec3(0.508273f, 0.508273f, 0.508273f);
float material_shininess= 51.2f;

float quadVertices[] = {
    // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // NOTE that this plane is now much smaller and at the top of the screen
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

float skyboxVertices[] = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};


bool ReadFile(const std::string filename, std::string *shader_source) {
  std::ifstream infile(filename.c_str());

  if (!infile.is_open() || !infile.good()) {
    std::cerr << "Error " + filename + " not found." << std::endl;
    return false;
  }

  std::stringstream stream;
  stream << infile.rdbuf();
  infile.close();

  *shader_source = stream.str();
  return true;
}

bool LoadImage(const std::string &path, GLuint cube_map_pos) {
  QImage image;
  std::cout<<path.c_str();
  bool res = image.load(path.c_str());
  image = image.convertToFormat(QImage::Format_RGB888);
  if (res) {
    QImage gl_image = image.mirrored();
    glTexImage2D(cube_map_pos, 0, GL_RGB, image.width(), image.height(), 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image.bits());
  }
  return res;
}

bool LoadProgram(const std::string &vertex, const std::string &fragment,
                 QOpenGLShaderProgram *program) {
  std::string vertex_shader, fragment_shader;
  bool res =
      ReadFile(vertex, &vertex_shader) && ReadFile(fragment, &fragment_shader);

  if (res) {
    program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                     vertex_shader.c_str());
    program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                     fragment_shader.c_str());
    program->bindAttributeLocation("vertex", kVertexAttributeIdx);
    program->bindAttributeLocation("normal", kNormalAttributeIdx);
    program->bindAttributeLocation("texture_coords", kTextureAttributeIdx);
    program->link();
  }

  return res;
}

}  // end of namespace

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent),
      initialized_(false),
      width_(0.0),
      height_(0.0),
      shader_mode_(0),
      fresnel_(0.972,0.960,0.915),
      metalness_(1.0),
      roughness_(0.10){
  setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget() {
  if (initialized_) {
    glDeleteTextures(1, &tex_map_albedo_);
    glDeleteTextures(1, &tex_map_metalness_);
    glDeleteTextures(1, &tex_map_roughness_);
    glDeleteTextures(1, &skybox_map_);
    glDeleteTextures(1, &env_cubemap_);
    glDeleteTextures(1, &diffuse_irradiance_map_);
    glDeleteTextures(1, &specular_irradiance_map_);
    glDeleteTextures(1, &diffuse_map_);
    glDeleteTextures(1, &specular_map_);
    glDeleteTextures(1, &tex_ssao_map_color_);
    glDeleteTextures(1, &tex_ssao_map_normal_);
    glDeleteTextures(1, &tex_ssao_map_depth_);
    glDeleteTextures(1, &tex_ssao_map_random_);
    glDeleteTextures(1, &tex_ssao_map_ssao_);
  }
}

bool GLWidget::LoadCubeMap(const QString &dir) {
  glGenVertexArrays(1, &sky_VAO);
  glBindVertexArray(sky_VAO);

  GLuint skyVBO;
  glGenBuffers(1, &skyVBO);
  glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

  std::string path = dir.toUtf8().constData();
  bool res = LoadImage(path + "/right.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
  res = res && LoadImage(path + "/left.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
  res = res && LoadImage(path + "/top.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
  res = res && LoadImage(path + "/bottom.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
  res = res && LoadImage(path + "/back.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
  res = res && LoadImage(path + "/front.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

  if (res) {
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    cubemap_path = path;
  }

  return res;
}

bool GLWidget::LoadModel(const QString &filename) {
  std::string file = filename.toUtf8().constData();
  size_t pos = file.find_last_of(".");
  std::string type = file.substr(pos + 1);

  std::unique_ptr<data_representation::TriangleMesh> mesh =
      std::make_unique<data_representation::TriangleMesh>();

  bool res = false;
  if (type.compare("ply") == 0) {
    res = data_representation::ReadFromPly(file, mesh.get());
  }

  if (res) {
    mesh_.reset(mesh.release());
    camera_.UpdateModel(mesh_->min_, mesh_->max_);

    // Create / Initialize buffers.
    glGenVertexArrays(1, &model_VAO);
    glBindVertexArray(model_VAO);

    GLuint VBO,nVBO,EBO,tVBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_->vertices_.size(), &mesh_->vertices_[0], GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_->faces_.size() * sizeof(unsigned int), &mesh_->faces_[0], GL_STATIC_DRAW);

    // vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

    // texture coords
    glGenBuffers(1, &tVBO);
    glBindBuffer(GL_ARRAY_BUFFER, tVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_->textures_.size(), &mesh_->textures_[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);


    glGenBuffers(1, &nVBO);
    glBindBuffer(GL_ARRAY_BUFFER, nVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_->normals_.size(), &mesh_->normals_[0], GL_STATIC_DRAW);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_map_albedo_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    LoadImage("../../ViewerPBS/textures/metal_spotty_discoloration/color.jpg", GL_TEXTURE_2D);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex_map_metalness_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    LoadImage("../../ViewerPBS/textures/metal_spotty_discoloration/metalness.jpg", GL_TEXTURE_2D);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex_map_roughness_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    LoadImage("../../ViewerPBS/textures/metal_spotty_discoloration/roughness.jpg", GL_TEXTURE_2D);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE12);
      glBindTexture(GL_TEXTURE_2D, tex_ssao_map_random_);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      LoadImage("../../ViewerPBS/textures/random_texture/noiseTexture.png", GL_TEXTURE_2D);
      glGenerateMipmap(GL_TEXTURE_2D);

      glBindVertexArray(0);
      glBindTexture(GL_TEXTURE_2D, 0);


    emit SetFaces(QString(std::to_string(mesh_->faces_.size() / 3).c_str()));
    emit SetVertices(
        QString(std::to_string(mesh_->vertices_.size() / 3).c_str()));
    return true;
  }

  return false;
}

bool GLWidget::LoadSkyboxMap(const QString &dir) {
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_map_);
  bool res = LoadCubeMap(dir);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return res;
}

bool GLWidget::LoadSpecularMap(const QString &dir) {
  glActiveTexture(GL_TEXTURE8);
  glBindTexture(GL_TEXTURE_CUBE_MAP, specular_map_);
  std::string path = dir.toUtf8().constData();
  bool res = LoadImage(path + "/right.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
  res = res && LoadImage(path + "/left.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
  res = res && LoadImage(path + "/top.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
  res = res && LoadImage(path + "/bottom.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
  res = res && LoadImage(path + "/back.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
  res = res && LoadImage(path + "/front.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

  if (res) {
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }
  //bool res = LoadCubeMap(dir);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return res;
}

bool GLWidget::LoadDiffuseMap(const QString &dir) {
  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_map_);
  std::string path = dir.toUtf8().constData();
  bool res = LoadImage(path + "/right.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
  res = res && LoadImage(path + "/left.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
  res = res && LoadImage(path + "/top.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
  res = res && LoadImage(path + "/bottom.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
  res = res && LoadImage(path + "/back.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
  res = res && LoadImage(path + "/front.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

  if (res) {
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }
  //bool res = LoadCubeMap(dir);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return res;
}

bool GLWidget::ComputeDiffuseIrradianceMap() {
    GLint dims[4] = {0};
    glGetIntegerv(GL_VIEWPORT, dims);
    GLint scrWidth = dims[2];
    GLint scrHeight = dims[3];

    // pbr: create a cubemap
    // --------------------------------------------------------------------------------
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, env_cubemap_);
    bool res = LoadImage(cubemap_path + "/right.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    res = res && LoadImage(cubemap_path + "/left.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    res = res && LoadImage(cubemap_path + "/top.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    res = res && LoadImage(cubemap_path + "/bottom.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    res = res && LoadImage(cubemap_path + "/back.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    res = res && LoadImage(cubemap_path + "/front.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    if (res) {
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    else
    {
        std::cout << "Failed to load diffuse irradiance image." << std::endl;
        return false;
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // pbr: setup framebuffer
    // ----------------------
    glGenFramebuffers(1, &captureDiffuseFBO);
    glGenRenderbuffers(1, &captureDiffuseRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureDiffuseFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureDiffuseRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 256, 256);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureDiffuseRBO);

    // pbr: create an irradiance cubemap
    // --------------------------------------------------------------------------------
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_irradiance_map_);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::dmat4x4 captureProjection = glm::perspective(90.0f, 1.0f, 0.1f, 10.0f);

    Eigen::Matrix4f eigen_capture_projection;
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 4; ++j)
        eigen_capture_projection(i, j) = static_cast<float>(captureProjection[j][i]);
    glm::dmat4x4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    GLint projection_location, view_location, environment_map_location;
    diffuse_irradiance_program_->bind();
    projection_location = diffuse_irradiance_program_->uniformLocation("projection");
    environment_map_location = diffuse_irradiance_program_->uniformLocation("environment_map");
    view_location = diffuse_irradiance_program_->uniformLocation("view");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_map_albedo_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex_map_metalness_);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex_map_roughness_);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_map_);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, env_cubemap_);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_irradiance_map_);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, specular_irradiance_map_);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_map_);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_CUBE_MAP, specular_map_);

    glUniformMatrix4fv(projection_location, 1, GL_FALSE, eigen_capture_projection.data());
    glUniform1i(environment_map_location, 4);

    glViewport(0, 0, 256, 256); // don't forget to configure the viewport to the capture dimensions.
    for (unsigned int i = 0; i < 6; ++i) {
        Eigen::Matrix4f eigen_capture_view;
        glm::dmat4x4 capture_view = captureViews[i];
        for (int j = 0; j < 4; ++j)
          for (int k = 0; k < 4; ++k)
            eigen_capture_view(j, k) = static_cast<float>(capture_view[k][j]);
        glUniformMatrix4fv(view_location, 1, GL_FALSE, eigen_capture_view.data());

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, diffuse_irradiance_map_, 0);
        glClearColor(1.0f,1.0f,1.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render Cube
        glBindVertexArray(sky_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        const int numberOfPixels = 256 * 256 * 4;
        unsigned char pixels[numberOfPixels];

        /// READ THE CONTENT FROM THE FBO
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glReadPixels(0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        QImage image = QImage(256,256,QImage::Format_RGBA8888);
        for (int x = 0; x < 256; ++x) {
            for (int y = 0; y < 256; ++y) {
                image.setPixel(x, y, qRgba(pixels[x*256*4+y*4], pixels[x*256*4+y*4+1], pixels[x*256*4+y*4+2], pixels[x*256*4+y*4+3]));
            }
        }
        if (i == 0) {
            image.save("../../ViewerPBS/textures/DiffuseIrradianceMap/right.png", "PNG"); // writes image into ba in PNG format
        } else if (i == 1) {
            image.save("../../ViewerPBS/textures/DiffuseIrradianceMap/left.png", "PNG"); // writes image into ba in PNG format
        } else if (i == 2) {
            image.save("../../ViewerPBS/textures/DiffuseIrradianceMap/top.png", "PNG"); // writes image into ba in PNG format
        } else if (i == 3) {
            image.save("../../ViewerPBS/textures/DiffuseIrradianceMap/bottom.png", "PNG"); // writes image into ba in PNG format
        } else if (i == 4) {
            image.save("../../ViewerPBS/textures/DiffuseIrradianceMap/back.png", "PNG"); // writes image into ba in PNG format
        } else if (i == 5) {
            image.save("../../ViewerPBS/textures/DiffuseIrradianceMap/front.png", "PNG"); // writes image into ba in PNG format
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    glViewport(0, 0, scrWidth, scrHeight);
    glDeleteFramebuffers(1, &captureDiffuseFBO);
    return true;
}

bool GLWidget::ComputeSpecularIrradianceMap() {
    GLint dims[4] = {0};
    glGetIntegerv(GL_VIEWPORT, dims);
    GLint scrWidth = dims[2];
    GLint scrHeight = dims[3];

    // pbr: create a cubemap
    // --------------------------------------------------------------------------------
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, env_cubemap_);
    bool res = LoadImage(cubemap_path + "/right.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    res = res && LoadImage(cubemap_path + "/left.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    res = res && LoadImage(cubemap_path + "/top.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    res = res && LoadImage(cubemap_path + "/bottom.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    res = res && LoadImage(cubemap_path + "/back.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    res = res && LoadImage(cubemap_path + "/front.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    if (res) {
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    else
    {
        std::cout << "Failed to load specular irradiance image." << std::endl;
        return false;
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // pbr: setup framebuffer
    // ----------------------
    glGenFramebuffers(1, &captureSpecularFBO);
    glGenRenderbuffers(1, &captureSpecularRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureSpecularFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureSpecularRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 256, 256);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureSpecularRBO);

    // pbr: create an irradiance cubemap
    // --------------------------------------------------------------------------------
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, specular_irradiance_map_);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::dmat4x4 captureProjection = glm::perspective(90.0f, 1.0f, 0.1f, 10.0f);

    Eigen::Matrix4f eigen_capture_projection;
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 4; ++j)
        eigen_capture_projection(i, j) = static_cast<float>(captureProjection[j][i]);
    glm::dmat4x4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // pbr: solve specular integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    GLint projection_location, view_location, environment_map_location, roughness_location;
    specular_irradiance_program_->bind();
    projection_location = specular_irradiance_program_->uniformLocation("projection");
    environment_map_location = specular_irradiance_program_->uniformLocation("environment_map");
    view_location = specular_irradiance_program_->uniformLocation("view");
    roughness_location = specular_irradiance_program_->uniformLocation("roughness");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_map_albedo_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex_map_metalness_);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex_map_roughness_);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_map_);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, env_cubemap_);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_irradiance_map_);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, specular_irradiance_map_);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_map_);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_CUBE_MAP, specular_map_);

    glUniformMatrix4fv(projection_location, 1, GL_FALSE, eigen_capture_projection.data());
    glUniform1i(environment_map_location, 4);
    glUniform1f(roughness_location, roughness_);

    glViewport(0, 0, 256, 256); // don't forget to configure the viewport to the capture dimensions.
    for (unsigned int i = 0; i < 6; ++i) {
        Eigen::Matrix4f eigen_capture_view;
        glm::dmat4x4 capture_view = captureViews[i];
        for (int j = 0; j < 4; ++j)
          for (int k = 0; k < 4; ++k)
            eigen_capture_view(j, k) = static_cast<float>(capture_view[k][j]);
        glUniformMatrix4fv(view_location, 1, GL_FALSE, eigen_capture_view.data());

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, specular_irradiance_map_, 0);
        glClearColor(1.0f,1.0f,1.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render Cube
        glBindVertexArray(sky_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        const int numberOfPixels = 256 * 256 * 4;
        unsigned char pixels[numberOfPixels];

        /// READ THE CONTENT FROM THE FBO
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glReadPixels(0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        QImage image = QImage(256,256,QImage::Format_RGBA8888);
        for (int x = 0; x < 256; ++x) {
            for (int y = 0; y < 256; ++y) {
                image.setPixel(x, y, qRgba(pixels[x*256*4+y*4], pixels[x*256*4+y*4+1], pixels[x*256*4+y*4+2], pixels[x*256*4+y*4+3]));
            }
        }
        if (i == 0) {
            image.save("../../ViewerPBS/textures/SpecularIrradianceMap/right.png", "PNG"); // writes image into ba in PNG format
        } else if (i == 1) {
            image.save("../../ViewerPBS/textures/SpecularIrradianceMap/left.png", "PNG"); // writes image into ba in PNG format
        } else if (i == 2) {
            image.save("../../ViewerPBS/textures/SpecularIrradianceMap/top.png", "PNG"); // writes image into ba in PNG format
        } else if (i == 3) {
            image.save("../../ViewerPBS/textures/SpecularIrradianceMap/bottom.png", "PNG"); // writes image into ba in PNG format
        } else if (i == 4) {
            image.save("../../ViewerPBS/textures/SpecularIrradianceMap/back.png", "PNG"); // writes image into ba in PNG format
        } else if (i == 5) {
            image.save("../../ViewerPBS/textures/SpecularIrradianceMap/front.png", "PNG"); // writes image into ba in PNG format
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    glViewport(0, 0, scrWidth, scrHeight);
    glDeleteFramebuffers(1, &captureSpecularFBO);
    return true;
}

void GLWidget::initializeGL() {

    ssao_n_samples_ = 64;
    ssao_radius_ = 0.001f;
    ssao_sigma_ = 1.8f;
    ssao_k_ = 2.5f;
    ssao_beta_ = 0.0001f;
    ssao_epsilon_ = 0.0001f;
    ssao_render_mode_ = 5;
    skybox_mode_ = 0;
  glewInit();

  glEnable(GL_NORMALIZE);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);

  glGenTextures(1, &tex_map_albedo_);
  glGenTextures(1, &tex_map_metalness_);
  glGenTextures(1, &tex_map_roughness_);
  glGenTextures(1, &skybox_map_);
  glGenTextures(1, &env_cubemap_);
  glGenTextures(1, &diffuse_irradiance_map_);
  glGenTextures(1, &specular_irradiance_map_);
  glGenTextures(1, &diffuse_map_);
  glGenTextures(1, &specular_map_);
  glGenTextures(1, &tex_ssao_map_color_);
  glGenTextures(1, &tex_ssao_map_normal_);
  glGenTextures(1, &tex_ssao_map_depth_);
  glGenTextures(1, &tex_ssao_map_random_);
  glGenTextures(1, &tex_ssao_map_ssao_);

  phong_program_ = std::make_unique<QOpenGLShaderProgram>();
  texture_mapping_color_program_ = std::make_unique<QOpenGLShaderProgram>();
  texture_mapping_metalness_program_ = std::make_unique<QOpenGLShaderProgram>();
  texture_mapping_roughness_program_ = std::make_unique<QOpenGLShaderProgram>();
  reflection_program_ = std::make_unique<QOpenGLShaderProgram>();
  brdf_program_ = std::make_unique<QOpenGLShaderProgram>();
  sky_program_ = std::make_unique<QOpenGLShaderProgram>();
  diffuse_irradiance_program_ = std::make_unique<QOpenGLShaderProgram>();
  specular_irradiance_program_ = std::make_unique<QOpenGLShaderProgram>();
  step_one_program_ = std::make_unique<QOpenGLShaderProgram>();
  step_two_program_ = std::make_unique<QOpenGLShaderProgram>();
  step_three_program_ = std::make_unique<QOpenGLShaderProgram>();
  step_four_program_ = std::make_unique<QOpenGLShaderProgram>();

  bool res =
          LoadProgram(kPhongVertexShaderFile, kPhongFragmentShaderFile,
                                     phong_program_.get());
  res = res && LoadProgram(kTextureMappingColorVertexShaderFile, kTextureMappingColorFragmentShaderFile,
                           texture_mapping_color_program_.get());
  res = res && LoadProgram(kTextureMappingMetalnessVertexShaderFile, kTextureMappingMetalnessFragmentShaderFile,
                           texture_mapping_metalness_program_.get());
  res = res && LoadProgram(kTextureMappingRoughnessVertexShaderFile, kTextureMappingRoughnessFragmentShaderFile,
                           texture_mapping_roughness_program_.get());
  res = res && LoadProgram(kBRDFVertexShaderFile, kBRDFFragmentShaderFile,
                           brdf_program_.get());
  res = res && LoadProgram(kReflectionVertexShaderFile, kReflectionFragmentShaderFile,
                           reflection_program_.get());
  res = res && LoadProgram(kSkyVertexShaderFile, kSkyFragmentShaderFile,
                           sky_program_.get());
  res = res && LoadProgram(kDiffuseIrradianceVertexShaderFile, kDiffuseIrradianceFragmentShaderFile,
                           diffuse_irradiance_program_.get());
  res = res && LoadProgram(kSpecularIrradianceVertexShaderFile, kSpecularIrradianceFragmentShaderFile,
                           specular_irradiance_program_.get());
  res = res && LoadProgram(kStepOneVertexShaderFile, kStepOneFragmentShaderFile,
                           step_one_program_.get());
  res = res && LoadProgram(kStepTwoVertexShaderFile, kStepTwoFragmentShaderFile,
                           step_two_program_.get());
  res = res && LoadProgram(kStepThreeVertexShaderFile, kStepThreeFragmentShaderFile,
                           step_three_program_.get());
  res = res && LoadProgram(kStepFourVertexShaderFile, kStepFourFragmentShaderFile,
                           step_four_program_.get());

  LoadModel("../../NewModels/PLY/dragon_vrip.ply");
  LoadSkyboxMap("../../ViewerPBS/textures/desert_specular/");
  LoadDiffuseMap("../../ViewerPBS/textures/desert_diffuse/");
  LoadSpecularMap("../../ViewerPBS/textures/desert_specular/");

  // pbr: setup framebuffer
  // ----------------------
  GLint dims[4] = {0};
  glGetIntegerv(GL_VIEWPORT, dims);
  //GLint scrWidth = dims[2];
  //GLint scrHeight = dims[3];
  GLint scrWidth = 600;
  GLint scrHeight = 600;

  /*************** First render step: Normal, Albedo, Depth ***************/
  // generate the textures where you will store the information
  // create a color attachment texture
  glGenTextures(1, &tex_ssao_map_color_);
  glBindTexture(GL_TEXTURE_2D, tex_ssao_map_color_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scrWidth, scrHeight,
               0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // create a normal attachment texture
  glGenTextures(1, &tex_ssao_map_normal_);
  glBindTexture(GL_TEXTURE_2D, tex_ssao_map_normal_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scrWidth, scrHeight,
               0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // create a depth attachment texture
  glGenTextures(1, &tex_ssao_map_depth_);
  glBindTexture(GL_TEXTURE_2D, tex_ssao_map_depth_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, scrWidth, scrHeight,
               0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


  // generate the FrameBuffers and attach the textures to them
  // generate the RenderBuffers and attach the textures to them
  glGenFramebuffers(1, &ssao_normal_FBO);
  glGenRenderbuffers(1, &ssao_depth_RBO);

  glBindFramebuffer(GL_FRAMEBUFFER, ssao_normal_FBO);
  glBindRenderbuffer(GL_RENDERBUFFER, ssao_depth_RBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, scrWidth, scrHeight); // use a single renderbuffer object for depth
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ssao_depth_RBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_ssao_map_normal_, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_ssao_map_depth_, 0);

  // define the array of color buffers where the fragment shader will draw into
  glDrawBuffer(GL_COLOR_ATTACHMENT0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      qDebug() << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /*************** Second render step: SSAO ***************/

  // Generate the structures (VAO, VBO) to render the quad
  glGenVertexArrays(1, &quad_VAO);
  glBindVertexArray(quad_VAO);

  GLuint quad_VBO;
  glGenBuffers(1, &quad_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, quad_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  // create a ssao attachment texture
  glGenTextures(1, &tex_ssao_map_ssao_);
  glBindTexture(GL_TEXTURE_2D, tex_ssao_map_ssao_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scrWidth, scrHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // generate the FrameBuffers and attach the textures to them
  glGenFramebuffers(1, &ssao_ssao_FBO);

  glBindFramebuffer(GL_FRAMEBUFFER, ssao_ssao_FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_ssao_map_ssao_, 0);

  // define the array of color buffers where the fragment shader will draw into
  glDrawBuffer(GL_COLOR_ATTACHMENT0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      qDebug() << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /*************** Third render step: SSAO Blur ***************/

  // create a ssao attachment texture
  glGenTextures(1, &tex_ssao_map_ssao_blur_);
  glBindTexture(GL_TEXTURE_2D, tex_ssao_map_ssao_blur_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scrWidth, scrHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // generate the FrameBuffers and attach the textures to them
  glGenFramebuffers(1, &ssao_ssao_blur_FBO);

  glBindFramebuffer(GL_FRAMEBUFFER, ssao_ssao_blur_FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_ssao_map_ssao_blur_, 0);

  // define the array of color buffers where the fragment shader will draw into
  glDrawBuffer(GL_COLOR_ATTACHMENT0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      qDebug() << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /*************** Fourth render step: Lightning Effects ***************/

  // create a ssao attachment texture
  glGenTextures(1, &tex_ssao_map_lightning_);
  glBindTexture(GL_TEXTURE_2D, tex_ssao_map_lightning_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scrWidth, scrHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // generate the FrameBuffers and attach the textures to them
  glGenFramebuffers(1, &ssao_ssao_lightning_FBO);

  glBindFramebuffer(GL_FRAMEBUFFER, ssao_ssao_lightning_FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_ssao_map_lightning_, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex_ssao_map_color_, 0);

  // define the array of color buffers where the fragment shader will draw into
  const GLenum buffers[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_DEPTH_ATTACHMENT};
  glDrawBuffers(2, buffers);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      qDebug() << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  if (!res) exit(0);

  //Kernel sampling
  float twopi = 2 * M_PI;
  for (unsigned int i = 0; i < 64; ++i)
  {
      float theta = twopi * randomFloats(*QRandomGenerator::global());        /* angle is uniform */
      float r = /*ssao_radius_ **/ sqrt(randomFloats(*QRandomGenerator::global()));   /* radius proportional to sqrt(U), U~U(0,1) */
      glm::vec2 sample(
          r*cos(theta),
          r*sin(theta)
      );
      ssao_kernel.push_back(sample);
  }

  initialized_ = true;
}

void GLWidget::resizeGL(int w, int h) {
  if (h == 0) h = 1;
  width_ = w;
  height_ = h;

  camera_.SetViewport(0, 0, w, h);
  camera_.SetProjection(kFieldOfView, kZNear, kZFar);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    camera_.StartRotating(event->x(), event->y());
  }
  if (event->button() == Qt::RightButton) {
    camera_.StartZooming(event->x(), event->y());
  }
  updateGL();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  camera_.SetRotationX(event->y());
  camera_.SetRotationY(event->x());
  camera_.SafeZoom(event->y());
  updateGL();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    camera_.StopRotating(event->x(), event->y());
  }
  if (event->button() == Qt::RightButton) {
    camera_.StopZooming(event->x(), event->y());
  }
  updateGL();
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Up) camera_.Zoom(-1);
  if (event->key() == Qt::Key_Down) camera_.Zoom(1);

  if (event->key() == Qt::Key_Left) camera_.Rotate(-1);
  if (event->key() == Qt::Key_Right) camera_.Rotate(1);

  if (event->key() == Qt::Key_W) camera_.Zoom(-1);
  if (event->key() == Qt::Key_S) camera_.Zoom(1);

  if (event->key() == Qt::Key_A) camera_.Rotate(-1);
  if (event->key() == Qt::Key_D) camera_.Rotate(1);

  if (event->key() == Qt::Key_R) {
    phong_program_.reset();
    phong_program_ = std::make_unique<QOpenGLShaderProgram>();
    LoadProgram(kPhongVertexShaderFile, kPhongFragmentShaderFile,
                phong_program_.get());
    texture_mapping_color_program_.reset();
    texture_mapping_color_program_ = std::make_unique<QOpenGLShaderProgram>();
    LoadProgram(kTextureMappingColorVertexShaderFile, kTextureMappingColorFragmentShaderFile,
                texture_mapping_color_program_.get());
    texture_mapping_metalness_program_.reset();
    texture_mapping_metalness_program_ = std::make_unique<QOpenGLShaderProgram>();
    LoadProgram(kTextureMappingMetalnessVertexShaderFile, kTextureMappingMetalnessFragmentShaderFile,
                texture_mapping_metalness_program_.get());
    texture_mapping_roughness_program_.reset();
    texture_mapping_roughness_program_ = std::make_unique<QOpenGLShaderProgram>();
    LoadProgram(kTextureMappingRoughnessVertexShaderFile, kTextureMappingRoughnessFragmentShaderFile,
                texture_mapping_roughness_program_.get());
    reflection_program_.reset();
    reflection_program_ = std::make_unique<QOpenGLShaderProgram>();
    LoadProgram(kReflectionVertexShaderFile, kReflectionFragmentShaderFile,
                reflection_program_.get());

    brdf_program_.reset();
    brdf_program_ = std::make_unique<QOpenGLShaderProgram>();
    LoadProgram(kBRDFVertexShaderFile, kBRDFFragmentShaderFile,
                brdf_program_.get());

    sky_program_.reset();
    sky_program_ = std::make_unique<QOpenGLShaderProgram>();
    LoadProgram(kSkyVertexShaderFile, kSkyFragmentShaderFile,
                sky_program_.get());

    diffuse_irradiance_program_.reset();
    diffuse_irradiance_program_ = std::make_unique<QOpenGLShaderProgram>();
    LoadProgram(kDiffuseIrradianceVertexShaderFile, kDiffuseIrradianceFragmentShaderFile,
                diffuse_irradiance_program_.get());

    specular_irradiance_program_.reset();
    specular_irradiance_program_ = std::make_unique<QOpenGLShaderProgram>();
    LoadProgram(kSpecularIrradianceVertexShaderFile, kSpecularIrradianceFragmentShaderFile,
                specular_irradiance_program_.get());

    step_one_program_.reset();
    step_one_program_ = std::make_unique<QOpenGLShaderProgram>();
    LoadProgram(kStepOneVertexShaderFile, kStepOneFragmentShaderFile,
                step_one_program_.get());

    step_two_program_.reset();
    step_two_program_ = std::make_unique<QOpenGLShaderProgram>();
    LoadProgram(kStepTwoVertexShaderFile, kStepTwoFragmentShaderFile,
                step_two_program_.get());

    step_three_program_.reset();
    step_three_program_ = std::make_unique<QOpenGLShaderProgram>();
    LoadProgram(kStepThreeVertexShaderFile, kStepThreeFragmentShaderFile,
                step_three_program_.get());

    step_four_program_.reset();
    step_four_program_ = std::make_unique<QOpenGLShaderProgram>();
    LoadProgram(kStepFourVertexShaderFile, kStepFourFragmentShaderFile,
                step_four_program_.get());
  }

  updateGL();
}

void GLWidget::paintGL() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glDepthFunc(GL_LEQUAL);

  if (initialized_) {
    camera_.SetViewport();

    Eigen::Matrix4f projection = camera_.SetProjection();
    Eigen::Matrix4f view = camera_.SetView();
    Eigen::Matrix4f inverse_view = view.inverse();
    Eigen::Matrix4f model = camera_.SetModel();

    Eigen::Matrix4f t = view * model;
    Eigen::Matrix3f normal;
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j) normal(i, j) = t(i, j);

    normal = normal.inverse().transpose();

    Eigen::Vector4f aux_light_position(light_position[0],light_position[1],light_position[2],1.0);
    aux_light_position = t * aux_light_position;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_map_albedo_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex_map_metalness_);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex_map_roughness_);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_map_);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, env_cubemap_);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_irradiance_map_);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, specular_irradiance_map_);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_map_);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_CUBE_MAP, specular_map_);
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, tex_ssao_map_color_);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, tex_ssao_map_normal_);
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, tex_ssao_map_depth_);
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, tex_ssao_map_random_);
    glActiveTexture(GL_TEXTURE13);
    glBindTexture(GL_TEXTURE_2D, tex_ssao_map_ssao_);
    glActiveTexture(GL_TEXTURE14);
    glBindTexture(GL_TEXTURE_2D, tex_ssao_map_ssao_blur_);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, tex_ssao_map_lightning_);


    /*************** First render step ***************/
    // bind the corresponding frame buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, ssao_normal_FBO);
    //clear the buffers (at least color and depth), send the uniforms, and draw the geometry.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GLint projection_location,
        view_location,
        inverse_view_location,
        model_location,
        normal_matrix_location,
        specular_map_location,
        diffuse_map_location,
        texture_chosen_location,
        fresnel_location,
        metalness_location,
        roughness_location,
        albedo_location,
        ao_location,
        light_position_location,
        light_color_location,
        material_ambient_location,
        material_diffuse_location,
        material_specular_location,
        material_shininess_location,
        texture_color_location,
        texture_metalness_location,
        texture_roughness_location,
        texture_ssao_albedo_location,
        texture_ssao_normal_location,
        texture_ssao_depth_location,
        texture_ssao_random_location,
        ssao_samples_location,
        ssao_n_samples_location,
        ssao_radius_location,
        ssao_sigma_location,
        ssao_k_location,
        ssao_beta_location,
        ssao_epsilon_location,
        ssao_render_mode_location,
        texture_ssao_ssao_location,
        texture_ssao_ssao_blur_location,
        texture_ssao_lightning_location;

    if (mesh_ != nullptr) {

        step_one_program_->bind();
        projection_location = step_one_program_->uniformLocation("projection");
        view_location = step_one_program_->uniformLocation("view");
        model_location = step_one_program_->uniformLocation("model");
        normal_matrix_location = step_one_program_->uniformLocation("normal_matrix");

        glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.data());
        glUniformMatrix4fv(view_location, 1, GL_FALSE, view.data());
        glUniformMatrix4fv(model_location, 1, GL_FALSE, model.data());
        glUniformMatrix3fv(normal_matrix_location, 1, GL_FALSE, normal.data());

      // Implement model rendering.
      // Activem l'Array a pintar
      glBindVertexArray(model_VAO);
      // Pintem l'escena
      glDrawElements(GL_TRIANGLES, mesh_->faces_.size(), GL_UNSIGNED_INT, 0);
      // Desactivem el model_VAO
      glBindVertexArray(0);
    }

    //glDepthFunc(GL_LEQUAL);

    /*sky_program_->bind();
    projection_location = sky_program_->uniformLocation("projection");
    view_location = sky_program_->uniformLocation("view");
    GLint skybox_map_location = sky_program_->uniformLocation("skybox_map_");

    glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.data());
    view(0,3) = 0.f;
    view(1,3) = 0.f;
    view(2,3) = 0.f;
    view(3,3) = 0.f;
    glUniformMatrix4fv(view_location, 1, GL_FALSE, view.data());

    glBindVertexArray(sky_VAO);
    glUniform1i(skybox_map_location, 3);*/
    // Implement the rendering of a bounding cube displaying the environment map.
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    //glDepthFunc(GL_LESS);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    /*************** Second render step ***************/
    // second render pass: draw as normal
    // ----------------------------------
    // bind the corresponding frame buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, ssao_ssao_FBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    step_two_program_->bind();
    projection_location = step_two_program_->uniformLocation("projection");
    view_location = step_two_program_->uniformLocation("view");
    inverse_view_location = step_two_program_->uniformLocation("inverse_view");
    model_location = step_two_program_->uniformLocation("model");
    normal_matrix_location = step_two_program_->uniformLocation("normal_matrix");
    texture_ssao_albedo_location = step_two_program_->uniformLocation("texture_ssao_albedo");
    texture_ssao_normal_location = step_two_program_->uniformLocation("texture_ssao_normal");
    texture_ssao_depth_location = step_two_program_->uniformLocation("texture_ssao_depth");
    texture_ssao_random_location = step_two_program_->uniformLocation("texture_ssao_random");
    ssao_samples_location = step_two_program_->uniformLocation("ssao_samples");
    ssao_n_samples_location = step_two_program_->uniformLocation("ssao_n_samples");
    ssao_radius_location = step_two_program_->uniformLocation("ssao_radius");
    ssao_sigma_location = step_two_program_->uniformLocation("ssao_sigma");
    ssao_k_location = step_two_program_->uniformLocation("ssao_k");
    ssao_beta_location = step_two_program_->uniformLocation("ssao_beta");
    ssao_epsilon_location = step_two_program_->uniformLocation("ssao_epsilon");
    ssao_render_mode_location = step_two_program_->uniformLocation("ssao_render_mode");
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.data());
    glUniformMatrix4fv(view_location, 1, GL_FALSE, view.data());
    glUniformMatrix4fv(model_location, 1, GL_FALSE, model.data());
    glUniformMatrix3fv(normal_matrix_location, 1, GL_FALSE, normal.data());
    glUniform1i(texture_ssao_albedo_location, 9);
    glUniform1i(texture_ssao_normal_location, 10);
    glUniform1i(texture_ssao_depth_location, 11);
    glUniform1i(texture_ssao_random_location, 12);

    glUniform2fv(ssao_samples_location,ssao_n_samples_,glm::value_ptr(ssao_kernel[0]));
    glUniform1i(ssao_n_samples_location,ssao_n_samples_);
    glUniform1f(ssao_radius_location,ssao_radius_);
    glUniform1f(ssao_sigma_location,ssao_sigma_);
    glUniform1f(ssao_k_location,ssao_k_);
    glUniform1f(ssao_beta_location,ssao_beta_);
    glUniform1f(ssao_epsilon_location,ssao_epsilon_);
    glUniform1i(ssao_render_mode_location,ssao_render_mode_);

    // now draw the mirror quad with screen texture
    // --------------------------------------------
    //glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

    glBindVertexArray(quad_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    /*************** Third render step ***************/
    glBindFramebuffer(GL_FRAMEBUFFER, ssao_ssao_blur_FBO);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    step_three_program_->bind();
    texture_ssao_albedo_location = step_three_program_->uniformLocation("texture_ssao_albedo");
    texture_ssao_normal_location = step_three_program_->uniformLocation("texture_ssao_normal");
    texture_ssao_depth_location = step_three_program_->uniformLocation("texture_ssao_depth");
    texture_ssao_ssao_location = step_three_program_->uniformLocation("texture_ssao_ssao");
    ssao_render_mode_location = step_three_program_->uniformLocation("ssao_render_mode");

    glUniform1i(texture_ssao_albedo_location, 9);
    glUniform1i(texture_ssao_normal_location, 10);
    glUniform1i(texture_ssao_depth_location, 11);
    glUniform1i(texture_ssao_ssao_location, 13);
    glUniform1i(ssao_render_mode_location,ssao_render_mode_);

    glBindVertexArray(quad_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    /*************** Fourth render step ***************/
    glBindFramebuffer(GL_FRAMEBUFFER, ssao_ssao_lightning_FBO);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (shader_mode_ == 0) {
      phong_program_->bind();
      projection_location = phong_program_->uniformLocation("projection");
      view_location = phong_program_->uniformLocation("view");
      model_location = phong_program_->uniformLocation("model");
      normal_matrix_location = phong_program_->uniformLocation("normal_matrix");
      light_position_location = phong_program_->uniformLocation("light_position");
      material_ambient_location = phong_program_->uniformLocation("material_ambient");
      material_diffuse_location = phong_program_->uniformLocation("material_diffuse");
      material_specular_location = phong_program_->uniformLocation("material_specular");
      material_shininess_location = phong_program_->uniformLocation("material_shininess");

      glUniform3f(material_ambient_location, material_ambient[0], material_ambient[1], material_ambient[2]);
      glUniform3f(material_diffuse_location, material_diffuse[0], material_diffuse[1], material_diffuse[2]);
      glUniform3f(material_specular_location, material_specular[0], material_specular[1], material_specular[2]);
      glUniform1f(material_shininess_location, material_shininess);
    } else if (shader_mode_ == 1) {
        if (texture_mapping_mode_ == 0) {
            texture_mapping_color_program_->bind();
            projection_location = texture_mapping_color_program_->uniformLocation("projection");
            view_location = texture_mapping_color_program_->uniformLocation("view");
            model_location = texture_mapping_color_program_->uniformLocation("model");
            normal_matrix_location = texture_mapping_color_program_->uniformLocation("normal_matrix");
            light_position_location = texture_mapping_color_program_->uniformLocation("light_position");
            material_ambient_location = texture_mapping_color_program_->uniformLocation("material_ambient");
            material_diffuse_location = texture_mapping_color_program_->uniformLocation("material_diffuse");
            material_specular_location = texture_mapping_color_program_->uniformLocation("material_specular");
            material_shininess_location = texture_mapping_color_program_->uniformLocation("material_shininess");
            texture_color_location = texture_mapping_color_program_->uniformLocation("texture_color");
        } else if (texture_mapping_mode_ == 1)  {
            texture_mapping_metalness_program_->bind();
            projection_location = texture_mapping_metalness_program_->uniformLocation("projection");
            view_location = texture_mapping_metalness_program_->uniformLocation("view");
            model_location = texture_mapping_metalness_program_->uniformLocation("model");
            normal_matrix_location = texture_mapping_metalness_program_->uniformLocation("normal_matrix");
            light_position_location = texture_mapping_metalness_program_->uniformLocation("light_position");
            material_ambient_location = texture_mapping_metalness_program_->uniformLocation("material_ambient");
            material_diffuse_location = texture_mapping_metalness_program_->uniformLocation("material_diffuse");
            material_specular_location = texture_mapping_metalness_program_->uniformLocation("material_specular");
            material_shininess_location = texture_mapping_metalness_program_->uniformLocation("material_shininess");
            texture_color_location = texture_mapping_metalness_program_->uniformLocation("texture_color");
            texture_metalness_location = texture_mapping_metalness_program_->uniformLocation("texture_metalness");

            glUniform1i(texture_metalness_location, 1);
        } else if (texture_mapping_mode_ == 2){
            texture_mapping_roughness_program_->bind();
            projection_location = texture_mapping_roughness_program_->uniformLocation("projection");
            view_location = texture_mapping_roughness_program_->uniformLocation("view");
            model_location = texture_mapping_roughness_program_->uniformLocation("model");
            normal_matrix_location = texture_mapping_roughness_program_->uniformLocation("normal_matrix");
            light_position_location = texture_mapping_roughness_program_->uniformLocation("light_position");
            material_ambient_location = texture_mapping_roughness_program_->uniformLocation("material_ambient");
            material_diffuse_location = texture_mapping_roughness_program_->uniformLocation("material_diffuse");
            material_specular_location = texture_mapping_roughness_program_->uniformLocation("material_specular");
            material_shininess_location = texture_mapping_roughness_program_->uniformLocation("material_shininess");
            texture_color_location = texture_mapping_roughness_program_->uniformLocation("texture_color");
            texture_roughness_location = texture_mapping_roughness_program_->uniformLocation("texture_roughness");


            glUniform1i(texture_roughness_location, 2);
        }
        glUniform1i(texture_color_location, 0);
        glUniform3f(material_ambient_location, material_ambient[0], material_ambient[1], material_ambient[2]);
        glUniform3f(material_diffuse_location, material_diffuse[0], material_diffuse[1], material_diffuse[2]);
        glUniform3f(material_specular_location, material_specular[0], material_specular[1], material_specular[2]);
        glUniform1f(material_shininess_location, material_shininess);
      } else if (shader_mode_ == 2) {
        reflection_program_->bind();
        projection_location = reflection_program_->uniformLocation("projection");
        view_location = reflection_program_->uniformLocation("view");
        inverse_view_location = reflection_program_->uniformLocation("inverse_view");
        model_location = reflection_program_->uniformLocation("model");
        normal_matrix_location = reflection_program_->uniformLocation("normal_matrix");
        texture_chosen_location = reflection_program_->uniformLocation("texture_chosen");

        glUniform1i(texture_chosen_location, 3);
        glUniformMatrix4fv(inverse_view_location, 1, GL_FALSE, inverse_view.data());
      } else {
      brdf_program_->bind();
      projection_location = brdf_program_->uniformLocation("projection");
      view_location = brdf_program_->uniformLocation("view");
      inverse_view_location = brdf_program_->uniformLocation("inverse_view");
      model_location = brdf_program_->uniformLocation("model");
      normal_matrix_location = brdf_program_->uniformLocation("normal_matrix");
      specular_map_location = brdf_program_->uniformLocation("specular_map");
      diffuse_map_location = brdf_program_->uniformLocation("diffuse_map");
      albedo_location = brdf_program_->uniformLocation("albedo");
      metalness_location = brdf_program_->uniformLocation("metalness");
      roughness_location = brdf_program_->uniformLocation("roughness");
      ao_location = brdf_program_->uniformLocation("ao");
      light_position_location = brdf_program_->uniformLocation("light_position");
      light_color_location = brdf_program_->uniformLocation("light_color");
      texture_color_location = brdf_program_->uniformLocation("texture_color");
      texture_roughness_location = brdf_program_->uniformLocation("texture_roughness");
      texture_metalness_location = brdf_program_->uniformLocation("texture_metalness");
      fresnel_location = brdf_program_->uniformLocation("fresnel");

      glUniform1i(diffuse_map_location, 7);
      glUniform1i(specular_map_location, 8);
      glUniform1i(texture_metalness_location, 1);
      glUniform1i(texture_roughness_location, 2);
      glUniformMatrix4fv(inverse_view_location, 1, GL_FALSE, inverse_view.data());
    }

    glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.data());
    glUniformMatrix4fv(view_location, 1, GL_FALSE, view.data());
    glUniformMatrix4fv(model_location, 1, GL_FALSE, model.data());
    glUniformMatrix3fv(normal_matrix_location, 1, GL_FALSE, normal.data());

    glUniform3f(albedo_location, 0.5f, 0.0f, 0.0f);
    glUniform1f(metalness_location, metalness_);
    glUniform1f(roughness_location, roughness_);
    glUniform1f(ao_location, 1.0f);

    glUniform3f(light_position_location, aux_light_position[0], aux_light_position[1], aux_light_position[2]);
    glUniform3f(light_color_location, light_color[0], light_color[1], light_color[2]);
    glUniform3f(fresnel_location, fresnel_[0], fresnel_[1], fresnel_[2]);

    if (mesh_ != nullptr) {

        /*step_one_program_->bind();
        projection_location = step_one_program_->uniformLocation("projection");
        view_location = step_one_program_->uniformLocation("view");
        model_location = step_one_program_->uniformLocation("model");
        normal_matrix_location = step_one_program_->uniformLocation("normal_matrix");*/

        glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.data());
        glUniformMatrix4fv(view_location, 1, GL_FALSE, view.data());
        glUniformMatrix4fv(model_location, 1, GL_FALSE, model.data());
        glUniformMatrix3fv(normal_matrix_location, 1, GL_FALSE, normal.data());

      // Implement model rendering.
      // Activem l'Array a pintar
      glBindVertexArray(model_VAO);
      // Pintem l'escena
      glDrawElements(GL_TRIANGLES, mesh_->faces_.size(), GL_UNSIGNED_INT, 0);
      // Desactivem el model_VAO
      glBindVertexArray(0);
    }

    glDepthFunc(GL_LEQUAL);

    if(skybox_mode_){
        sky_program_->bind();
        projection_location = sky_program_->uniformLocation("projection");
        view_location = sky_program_->uniformLocation("view");
        GLint skybox_map_location = sky_program_->uniformLocation("skybox_map_");

        glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.data());
        view(0,3) = 0.f;
        view(1,3) = 0.f;
        view(2,3) = 0.f;
        view(3,3) = 0.f;
        glUniformMatrix4fv(view_location, 1, GL_FALSE, view.data());

        glBindVertexArray(sky_VAO);
        glUniform1i(skybox_map_location, 3);
        // Implement the rendering of a bounding cube displaying the environment map.
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);
        glBindVertexArray(0);

    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    /*************** Fifth render step ***************/
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    step_four_program_->bind();
    texture_ssao_albedo_location = step_four_program_->uniformLocation("texture_ssao_albedo");
    texture_ssao_normal_location = step_four_program_->uniformLocation("texture_ssao_normal");
    texture_ssao_depth_location = step_four_program_->uniformLocation("texture_ssao_depth");
    texture_ssao_ssao_location = step_four_program_->uniformLocation("texture_ssao_ssao");
    texture_ssao_ssao_blur_location = step_four_program_->uniformLocation("texture_ssao_ssao_blur");
    texture_ssao_lightning_location = step_four_program_->uniformLocation("texture_ssao_lightning");
    ssao_render_mode_location = step_four_program_->uniformLocation("ssao_render_mode");

    glUniform1i(texture_ssao_albedo_location, 9);
    glUniform1i(texture_ssao_normal_location, 10);
    glUniform1i(texture_ssao_depth_location, 11);
    glUniform1i(texture_ssao_ssao_location, 13);
    glUniform1i(texture_ssao_ssao_blur_location, 14);
    glUniform1i(texture_ssao_lightning_location, 15);
    glUniform1i(ssao_render_mode_location,ssao_render_mode_);

    glBindVertexArray(quad_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

  }
}

void GLWidget::SetPhong(bool set) {
  shader_mode_ = 0;
  updateGL();
}

void GLWidget::SetTextureMapping(bool set) {
  shader_mode_ = 1;
  updateGL();
}

void GLWidget::SetReflection(bool set) {
  shader_mode_ = 2;
  updateGL();
}

void GLWidget::SetBRDF(bool set) {
  shader_mode_ = 3;
  updateGL();
}

void GLWidget::SetFresnelR(double r) {
  fresnel_[0] = r;
  updateGL();
}

void GLWidget::SetFresnelG(double g) {
  fresnel_[1] = g;
  updateGL();
}

void GLWidget::SetFresnelB(double b) {
  fresnel_[2] = b;
  updateGL();
}

void GLWidget::SetMetalness(int b) {
  metalness_= b/100.f;
  updateGL();
}

void GLWidget::SetRoughness(int b) {
  roughness_ = b/100.f;
  updateGL();
}

void GLWidget::SetTextureMappingMode(int mode) {
  texture_mapping_mode_ = mode;
  updateGL();
}

void GLWidget::SetSkybox(bool mode) {
  skybox_mode_ = mode;
  updateGL();
}

void GLWidget::SetSSAONormal(bool mode) {
  ssao_render_mode_ = 0;
  updateGL();
}

void GLWidget::SetSSAOAlbedo(bool mode) {
  ssao_render_mode_ = 1;
  updateGL();
}

void GLWidget::SetSSAODepth(bool mode) {
  ssao_render_mode_ = 2;
  updateGL();
}

void GLWidget::SetSSAOSSAO(bool mode) {
  ssao_render_mode_ = 3;
  updateGL();
}

void GLWidget::SetSSAOSSAOBlur(bool mode) {
  ssao_render_mode_ = 4;
  updateGL();
}

void GLWidget::SetSSAOSSAOBlurLightning(bool mode) {
  ssao_render_mode_ = 5;
  updateGL();
}

void GLWidget::SetSSAONSamples(int n_samples) {
  ssao_n_samples_ = n_samples;
  updateGL();
}

void GLWidget::SetSSAORadius(double radius) {
  ssao_radius_ = radius;
  updateGL();
}

void GLWidget::SetSSAOSigma(double sigma) {
  ssao_sigma_ = sigma;
  updateGL();
}

void GLWidget::SetSSAOK(double k) {
  ssao_k_ = k;
  updateGL();
}

void GLWidget::SetSSAOBeta(double beta) {
  ssao_beta_ = beta;
  updateGL();
}

void GLWidget::SetSSAOEpsilon(double epsilon) {
  ssao_epsilon_ = epsilon;
  updateGL();
}
