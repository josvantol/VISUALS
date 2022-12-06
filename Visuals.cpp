/*
     V I S U A L S

  Application for running video with live stage performance.
  Version 2.2 (December 6, 2022)
  by JVT

  Needs openFrameworks to build.
  Needs libfmod to run.

*/

#include "Visuals.h"

#define u32 unsigned int
#define s32 signed int
#define r32 float

#define internal static
#define global static

#define FRAME_RATE      15.0f
#define SECONDS_TO_FADE 2.0f
#define DEBUG 0

global ofDirectory Folder;
global ofVideoPlayer VideoA;
global ofVideoPlayer VideoB;

global u32 CurrentVideo;
global u32 FramesToFade;
global u32 FadeCounter;
global r32 Alpha;

global bool ButtonDown;
global bool Playing;
global bool LoadingA;
global bool LoadingB;
global bool FadeA;
global bool FadeB;

typedef enum application_state
{
  Main,
  Playlist,
  Help,
  Test
} application_state;

application_state State;

typedef enum video_player
{
  A,
  B
} video_player;

video_player CurrentPlayer;

internal void
DebugInfo()
{
  cout << "-- VISUALS DUBUG INFO --" << endl;
  cout << "> CurrentVideo: " << CurrentVideo << " (" << Folder.getPath(CurrentVideo) << ")" << endl;

  if(CurrentPlayer == A) cout << "> CurrentPlayer: A" << endl;
  else if(CurrentPlayer == B) cout << "> CurrentPlayer: B" << endl;
  else cout << "> CurrentPlayer: ERROR" << endl;

  if(!FadeA && !FadeB) cout << "> Not fadeing." << endl;
  else if (FadeA && !FadeB) cout << "> Fadeing video A: " << (Alpha / 255.0f * 100.0f) << endl;
  else if (!FadeA && FadeB) cout << "> Fadeing video B: " << (Alpha / 255.0f * 100.0f) << endl;
  else cout << "> Fadeing ERROR" << endl;
}

internal void
LoadVideo(u32 Index)
{
  if(DEBUG) cout << "VISUALS: Loading video index " << Index << endl;
  if(Index < Folder.size())
  {
    if(CurrentPlayer == A)
    {
      VideoB.load(Folder.getPath(Index));
      VideoB.setVolume(0.0);
      VideoB.setLoopState(OF_LOOP_NONE);
      VideoB.play();
      LoadingB = true;
      CurrentPlayer = B;
    }
    else
    {
      VideoA.load(Folder.getPath(Index));
      VideoA.setVolume(0.0);
      VideoA.setLoopState(OF_LOOP_NONE);
      VideoA.play();
      LoadingA = true;
      CurrentPlayer = A;
    }

    CurrentVideo = Index;
  }
  else
  {
    cout << "ERROR: No video available at index." << endl;
  }
}

internal void
DrawVideo()
{
  if(CurrentPlayer == A)
  {
    ofSetColor(255, 255, 255, 255);
    VideoA.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    if(FadeB)
    {
      ofSetColor(255, 255, 255, Alpha);
      VideoB.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    }
  }
  if(CurrentPlayer == B)
  {
    ofSetColor(255, 255, 255, 255);
    VideoB.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    if(FadeA)
    {
      ofSetColor(255, 255, 255, Alpha);
      VideoA.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    }
  }
}

internal void
DrawTestGraphics()
{
  int w = ofGetWindowWidth();
  int h = ofGetWindowHeight();

  int s = w/16;
  for(int Hpos = 0; Hpos < h; Hpos += s*2)
  {
    for(int Wpos = 0; Wpos < w; Wpos += s*2)
    {
      ofSetHexColor(0x808080);
      ofDrawRectangle(Wpos, Hpos, s, s);
    }
    for(int Wpos = s; Wpos < w; Wpos += s*2)
    {
      ofSetHexColor(0x808080);
      ofDrawRectangle(Wpos, Hpos+s, s, s);
    }
  }

  ofSetHexColor(0xFF0000);
  ofDrawRectangle(w/5, h/2/2, w/5, h/2);
  ofSetHexColor(0x00F000);
  ofDrawRectangle(w/5*2, h/2/2, w/5, h/2);
  ofSetHexColor(0x0000FF);
  ofDrawRectangle(w/5*3, h/2/2, w/5, h/2);

  ofSetHexColor(0xFFFFFF);
  ofSetLineWidth(4);
  ofDrawLine(w/2, 0, w/2, h);
  ofDrawLine(0, h/2, w, h/2);
  ofDrawLine(0, 0, w, 0);
  ofDrawLine(w, 0, w, h);
  ofDrawLine(w, h, 0, h);
  ofDrawLine(0, h, 0, 0);

  int x = h*0.03f;
  ofDrawTriangle(w/4, 0, w/4-x, x*2, w/4+x, x*2);
  ofDrawTriangle(w-w/4, 0, w-w/4-x, x*2, w-w/4+x, x*2);
  ofDrawTriangle(w/4, h, w/4-x, h - x*2, w/4+x, h - x*2);
  ofDrawTriangle(w-w/4, h, w-w/4-x, h-x*2, w-w/4+x, h-x*2);
  ofDrawTriangle(0, h/4, x*2, h/4-x, x*2, h/4+x);
  ofDrawTriangle(0, h-h/4, x*2, h-h/4-x, x*2, h-h/4+x);
  ofDrawTriangle(w, h/4, w-x*2, h/4-x, w-x*2, h/4+x);
  ofDrawTriangle(w, h-h/4, w-x*2, h-h/4-x, w-x*2, h-h/4+x);

  string Size = to_string(w) + " x " + to_string(h);
  ofDrawBitmapStringHighlight(Size, w/2+10, h/2+20);
}

void Visuals::setup()
{
  ofHideCursor();
  ofSetEscapeQuitsApp(false);
  ofSetFrameRate(FRAME_RATE);

  ButtonDown = false;
  Playing = true;
  FadeA = false;
  FadeB = false;
  CurrentVideo = 0;
  FramesToFade = ofGetFrameRate() * SECONDS_TO_FADE;
  FadeCounter = 0;
  Alpha = 255.0f;
  CurrentPlayer = A;
  State = Main;

  Folder.open("");
  Folder.allowExt("mp4");
  Folder.listDir();
  if (Folder.size() == 0)
  {
    cout << "ERROR: No MP4-files found in \"data\" folder." << endl;
    cout << "ERROR: Exiting..." << endl;
    ofExit();
  }
  else
  {
    Folder.sort();

    LoadVideo(0);
  }
}

void Visuals::update()
{
  FramesToFade = ofGetFrameRate() * SECONDS_TO_FADE;
  if(LoadingA && VideoA.isLoaded())
  {
    LoadingA = false;
    FadeB = true;
  }
  if(LoadingB && VideoB.isLoaded())
  {
    LoadingB = false;
    FadeA = true;
  }

  if(FadeA || FadeB)
  {
    Alpha = 255.0f - (r32)FadeCounter * (255.0f / (r32)FramesToFade);
    FadeCounter++;

    if(Alpha <= 0.0f)
    {
      if(CurrentPlayer == A)
      {
        VideoB.stop();
        VideoB.close();
      }
      if(CurrentPlayer == B)
      {
        VideoA.stop();
        VideoA.close();
      }
      FadeA = false;
      FadeB = false;
      FadeCounter = 0;
    }
  }

  if(CurrentPlayer == A || FadeA) VideoA.update();
  if(CurrentPlayer == B || FadeB) VideoB.update();
}

void Visuals::draw()
{
  ofBackground(0);

  switch (State)
  {
    case Main:
    {
      DrawVideo();
      break;
    }

    case Help:
    {
      DrawVideo();

      string HelpString = "VISUALS 2.2 by JVT\n\nHelp and info (F1)\n---------------------------\n\nESC: Leave fullscreen / Close application\n\nSPACE: Fade to next video in playlist\n(SHIFT +) 0 - 9: Start video in playlist (SHIFT adds 10)\n\nF1: This help\nF2: Print playlist\nF12: Test image\nP: Pause playback\nF: Toggle fullscreen";
      if (DEBUG)
      {
        HelpString += "\n\nVersion 2.2 Linux: December 6, 2022 (DEBUG Build)\n\nD: Print current state debug info to console.\n\nTODO:\n-SHIFT # takes two tries before catching."; 
      }
      ofDrawBitmapStringHighlight(HelpString, 100, 100);

      if(!Playing)
      {
        ofDrawBitmapStringHighlight("PAUSED", ofGetWindowWidth()/2, ofGetWindowHeight()/2);
      }

      break;
    }

    case Playlist:
    {
      DrawVideo();

      string Output = "TRACK LIST (F2)\n\n------------------------\n\n";
      for(u32 TrackIndex = 0; TrackIndex < Folder.size(); TrackIndex++)
      {
        if(TrackIndex == CurrentVideo)
        {
          Output += "> ";
        }
        else
        {
          Output += "  ";
        }
        Output += to_string(TrackIndex + 1) + ": " + Folder.getPath(TrackIndex) + "\n";
      }

      ofDrawBitmapStringHighlight(Output, 100, 100);

      break;
    }

    case Test:
    {
      DrawTestGraphics();
      break;
    }
  }

  if (DEBUG) ofDrawBitmapStringHighlight("DEBUG BUILD", 10, ofGetWindowHeight() - 10);
}

void Visuals::keyPressed(int key)
{
  if(ButtonDown == false)
  {
    ButtonDown = true;

    if(key == 'd')
    {
      DebugInfo();
    }

    if(key == OF_KEY_ESC)
    {
      if(ofGetWindowMode())
      {
        ofToggleFullscreen();
      }
      else
      {
        ofExit();
      }
    }

    if(key == ' ')
    {
      if(FadeA || FadeB) return;
      LoadVideo(CurrentVideo + 1);
    }

    if(key >= '1' && key <= '9')
    {
      if(FadeA || FadeB) return;
      LoadVideo(key - '0' - 1);
    }

    switch(key) // UK KEYBOARD SHIFT 0-9 KEYS
    {
      case '0':
        LoadVideo(9);
        break;
      case 33: // '!'
        LoadVideo(10);
        break;
      case 34: // '"'
        LoadVideo(11);
        break;
      case 163: // pound symbol
        LoadVideo(12);
        break;
      case 36: // '$'
        LoadVideo(13);
        break;
      case 37: // '%'
        LoadVideo(14);
        break;
      case 94: // '^'
        LoadVideo(15);
        break;
      case 38: // '&'
        LoadVideo(16);
        break;
      case 42: // '*'
        LoadVideo(17);
        break;
      case 40: // '('
        LoadVideo(18);
        break;
      case 41: // ')'
        LoadVideo(19);
        break;
    }

    if(key == 'f')
    {
      ofToggleFullscreen();
    }

    if(key == 'p')
    {
      if(Playing)
      {
        VideoA.setPaused(true);
        VideoB.setPaused(true);
        Playing = false;
      }
      else
      {
        VideoA.setPaused(false);
        VideoB.setPaused(false);
        Playing = true;
      }
    }

    if(key == OF_KEY_F1)
    {
      if(State == Help)
      {
        State = Main;
      }
      else
      {
        State = Help;
      }
    }

    if(key == OF_KEY_F2)
    {
      if(State == Playlist)
      {
        State = Main;
      }
      else
      {
        State = Playlist;
      }
    }

    if(key == OF_KEY_F12)
    {
      if(State == Test)
      {
        State = Main;
      }
      else
      {
        State = Test;
      }
    }
  }
}

void Visuals::keyReleased(int key)
{
  ButtonDown = false;
}
