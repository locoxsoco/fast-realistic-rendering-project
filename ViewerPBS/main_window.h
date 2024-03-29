// Author: Marc Comino 2020

#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace gui {

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  virtual void show();

 private slots:
  /**
   * @brief on_actionQuit_triggered Closes the application.
   */
  void on_actionQuit_triggered();

  /**
   * @brief on_actionLoad_triggered Opens a file dialog to load a PLY mesh.
   */
  void on_actionLoad_Model_triggered();

  void on_actionLoad_Skybox_triggered();

  void on_actionCompute_Diffuse_Irrandiance_Map_triggered();
  void on_actionCompute_Specular_Irrandiance_Map_triggered();

  /**
   * @brief on_actionLoad_Specular_triggered Opens a file dialog to load a cube
   * map that will be used for the specular component.
   */
  void on_actionLoad_Specular_triggered();

  /**
   * @brief on_actionLoad_Diffuse_triggered Opens a file dialog to load a cube
   * map that will be used for the diffuse component.
   */
  void on_actionLoad_Diffuse_triggered();

 private:
  Ui::MainWindow *ui;
};

}  //  namespace gui

#endif  //  MAIN_WINDOW_H_
