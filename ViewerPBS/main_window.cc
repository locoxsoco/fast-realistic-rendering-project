// Author: Marc Comino 2020

#include <main_window.h>

#include <QFileDialog>
#include <QMessageBox>
#include "./ui_main_window.h"

namespace gui {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->texture_mapping_type->addItem("Color");
  ui->texture_mapping_type->addItem("Metalness");
  ui->texture_mapping_type->addItem("Roughness");
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::show() { QMainWindow::show(); }

void MainWindow::on_actionQuit_triggered() { close(); }

void MainWindow::on_actionLoad_Model_triggered() {
  QString filename;

  filename = QFileDialog::getOpenFileName(this, tr("Load model"), "./",
                                          tr("PLY Files ( *.ply )"));
  if (!filename.isNull()) {
    if (!ui->glwidget->LoadModel(filename))
      QMessageBox::warning(this, tr("Error"),
                           tr("The file could not be opened"));
  }
}

void MainWindow::on_actionLoad_Skybox_triggered() {
  QString dir =
      QFileDialog::getExistingDirectory(this, "Skybox CubeMap folder.", "./");
  if (!dir.isEmpty()) {
    if (!ui->glwidget->LoadSkyboxMap(dir))
      QMessageBox::warning(this, tr("Error"),
                           tr("The file could not be opened"));
  }
}

void MainWindow::on_actionLoad_Specular_triggered() {
  QString dir =
      QFileDialog::getExistingDirectory(this, "Specular CubeMap folder.", "./");
  if (!dir.isEmpty()) {
    if (!ui->glwidget->LoadSpecularMap(dir))
      QMessageBox::warning(this, tr("Error"),
                           tr("The file could not be opened"));
  }
}

void MainWindow::on_actionLoad_Diffuse_triggered() {
  QString dir =
      QFileDialog::getExistingDirectory(this, "Diffuse CubeMap folder.", "./");
  if (!dir.isEmpty()) {
    if (!ui->glwidget->LoadDiffuseMap(dir))
      QMessageBox::warning(this, tr("Error"),
                           tr("The file could not be opened"));
  }
}

void MainWindow::on_actionCompute_Diffuse_Irrandiance_Map_triggered() {
    if (!ui->glwidget->ComputeDiffuseIrradianceMap())
      QMessageBox::warning(this, tr("Error"),
                           tr("The Diffuse Irrandiance map could not be computed"));
}

void MainWindow::on_actionCompute_Specular_Irrandiance_Map_triggered() {
    if (!ui->glwidget->ComputeSpecularIrradianceMap())
      QMessageBox::warning(this, tr("Error"),
                           tr("The Specular Irrandiance map could not be computed"));
}

}  //  namespace gui
