/*
     V I S U A L S

  Application for running video with live stage performance.
  Version 2.2 (December 6, 2022)
  by JVT

  Needs openFrameworks to build.
  Needs libfmod to run.

*/

#pragma once

#include "ofMain.h"

class Visuals : public ofBaseApp
{
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
};
