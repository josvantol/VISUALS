/*
     V I S U A L S

  Application for running video with live stage performance.
  Version 2.2 (December 6, 2022)
  by JVT

  Needs openFrameworks to build.
  Needs libfmod to run.

*/

#include "ofMain.h"
#include "Visuals.h"

int main()
{
	ofSetupOpenGL(1024,768,OF_WINDOW);
	ofRunApp(new Visuals());
}
