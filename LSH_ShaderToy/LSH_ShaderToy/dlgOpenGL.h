#pragma once

void loadDlgOpenGL();
void freeDlgOpenGL();

void showDlgOpenGL();

void init();
void reshape(int w, int h);
void loop();

extern const char* strVertexShader;
extern const char* strFragShader;