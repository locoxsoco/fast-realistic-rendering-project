// Author: Marc Comino 2020

#ifndef GLWIDGET_H_
#define GLWIDGET_H_

#include <GL/glew.h>
#include <QGLWidget>
#include <QImage>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QString>

#include <memory>

#include "./camera.h"
#include "./triangle_mesh.h"

class GLWidget : public QGLWidget {
  Q_OBJECT

 public:
  explicit GLWidget(QWidget *parent = nullptr);
  ~GLWidget();

  /**
   * @brief LoadModel Loads a PLY model at the filename path into the mesh_ data
   * structure.
   * @param filename Path to the PLY model.
   * @return Whether it was able to load the model.
   */
  bool LoadModel(const QString &filename);

  /**
   * @brief LoadSpecularMap Will load load a cube map that will be used for the
   * specular component.
   * @param filename Path to the directory containing the 6 textures (right,
   * left, top, bottom, front back) of the sube map that will be used for the
   * specular component.
   * @return Whether it was able to load the textures.
   */
  bool LoadSpecularMap(const QString &filename);
  bool LoadSkyboxMap(const QString &filename);

  /**
   * @brief LoadDiffuseMap Will load load a cube map that will be used for the
   * specular component.
   * @param filename Path to the directory containing the 6 textures (right,
   * left, top, bottom, front back) of the sube map that will be used for the
   * diffuse component.
   * @return Whether it was able to load the textures.
   */
  bool LoadDiffuseMap(const QString &filename);

  bool ComputeDiffuseIrradianceMap();
  bool ComputeSpecularIrradianceMap();

  bool LoadCubeMap(const QString &filename);

 protected:
  /**
   * @brief initializeGL Initializes OpenGL variables and loads, compiles and
   * links shaders.
   */
  void initializeGL();

  /**
   * @brief resizeGL Resizes the viewport.
   * @param w New viewport width.
   * @param h New viewport height.
   */
  void resizeGL(int w, int h);

  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void keyPressEvent(QKeyEvent *event);

 private:
  std::unique_ptr<QOpenGLShaderProgram> phong_program_,
                                        texture_mapping_color_program_,
                                        texture_mapping_metalness_program_,
                                        texture_mapping_roughness_program_,
                                        step_one_program_,
                                        step_two_program_,
                                        step_three_program_,
                                        step_four_program_;
  /**
   * @brief program_ The reflection shader program.
   */
  std::unique_ptr<QOpenGLShaderProgram> reflection_program_;

  /**
   * @brief program_ The brdf shader program.
   */
  std::unique_ptr<QOpenGLShaderProgram> brdf_program_;


  /**
   * @brief program_ The skybox shader program.
   */
  std::unique_ptr<QOpenGLShaderProgram> sky_program_;

  std::unique_ptr<QOpenGLShaderProgram> diffuse_irradiance_program_;
  std::unique_ptr<QOpenGLShaderProgram> specular_irradiance_program_;

  /**
   * @brief camera_ Class that computes the multiple camera transform matrices.
   */
  data_visualization::Camera camera_;

  /**
   * @brief mesh_ Data structure representing a triangle mesh.
   */
  std::unique_ptr<data_representation::TriangleMesh> mesh_;

  /**
   * @brief diffuse_map_ Diffuse cubemap texture.
   */
  GLuint diffuse_map_;

  /**
   * @brief specular_map_ Diffuse cubemap texture.
   */
  GLuint specular_map_;

  /**
   * @brief initialized_ Whether the widget has finished initializations.
   */
  bool initialized_;

  /**
   * @brief width_ Viewport current width.
   */
  float width_;

  /**
   * @brief height_ Viewport current height.
   */
  float height_;

  /**
   * @brief reflection_ Whether to use the reflection shader or the brdf shader.
   */
  unsigned int shader_mode_,texture_mapping_mode_, ssao_render_mode_, skybox_mode_;
  std::string cubemap_path;

  /**
   * @brief fresnel_ Fresnel F0 color components.
   */
  Eigen::Vector3d fresnel_;
  float metalness_, roughness_;

  GLuint sky_VAO, model_VAO, quad_VAO;
  GLuint skybox_map_, tex_map_albedo_, tex_map_metalness_, tex_map_roughness_, env_cubemap_, diffuse_irradiance_map_, specular_irradiance_map_, tex_ssao_map_color_,tex_ssao_map_normal_, tex_ssao_map_depth_, tex_ssao_map_random_, tex_ssao_map_ssao_, tex_ssao_map_ssao_blur_, tex_ssao_map_lightning_;
  GLuint captureDiffuseFBO, captureDiffuseRBO, captureSpecularFBO, captureSpecularRBO, ssao_normal_FBO, ssao_depth_RBO, ssao_ssao_FBO, ssao_ssao_blur_FBO, ssao_ssao_lightning_FBO;

  int ssao_n_samples_;
  float ssao_radius_,ssao_sigma_,ssao_k_,ssao_beta_,ssao_epsilon_;

 protected slots:
  /**
   * @brief paintGL Function that handles rendering the scene.
   */
  void paintGL();

  /**
   * @brief SetReflection Enables the reflection shader.
   */
  void SetPhong(bool set);
  void SetTextureMapping(bool set);
  void SetReflection(bool set);

  /**
   * @brief SetReflection Enables the brdf shader.
   */
  void SetBRDF(bool set);

  /**
   * @brief SetFresnelR Sets the fresnel F0 red component.
   */
  void SetFresnelR(double);

  /**
   * @brief SetFresnelB Sets the fresnel F0 blue component.
   */
  void SetFresnelB(double);

  /**
   * @brief SetFresnelG Sets the fresnel F0 green component.
   */
  void SetFresnelG(double);

  void SetMetalness(int);
  void SetRoughness(int);
  void SetTextureMappingMode(int);
  void SetSkybox(bool);

  void SetSSAONormal(bool);
  void SetSSAOAlbedo(bool);
  void SetSSAODepth(bool);
  void SetSSAOSSAO(bool);
  void SetSSAOSSAOBlur(bool);
  void SetSSAOSSAOBlurLightning(bool);

  void SetSSAONSamples(int);
  void SetSSAORadius(double);
  void SetSSAOSigma(double);
  void SetSSAOK(double);
  void SetSSAOBeta(double);
  void SetSSAOEpsilon(double);

 signals:
  /**
   * @brief SetFaces Signal that updates the interface label "Faces".
   */
  void SetFaces(QString);

  /**
   * @brief SetFaces Signal that updates the interface label "Vertices".
   */
  void SetVertices(QString);

  /**
   * @brief SetFaces Signal that updates the interface label "Framerate".
   */
  void SetFramerate(QString);
};

#endif  //  GLWIDGET_H_
