/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi GUI Manager Implementation (guiman.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	GUIMAN.CPP v0.0
////
////	GUI Manager for GiPSi
////
////////////////////////////////////////////////////////////////


// Standard includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <GL/glut.h>
#include <GL/gl.h>

// GiPSi includes
#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "guiman.h"
#include "timing.h"



// GUIMan's Private Globals
SimulationKernel	*Sim;

int			main_window;			// handle for the main window
int			win_height, win_width;
int			win_xpos, win_ypos;

typedef struct {
	float x, y, z;
	float fx, fy, fz;
	float upx, upy, upz;
	float xAngle, yAngle;
	float prev_xAngle, prev_yAngle;
	float r, prev_r;
} Camera;

Camera camera = {	0.0, 0.0, 10.0, 
					0.0, 0.0, 0.0, 
					0.0, 1.0, 0.0,
					0.0, 0.0,
					0.0, 0.0,
					5.0, 5.0};

typedef struct {
	float pos[4];
} Light;

Light light0 = { { 0.0, 1.0, 2.0, 0.0 } };
Light light1 = { { sqrt(3.0), 1.0, -1.0, 0.0 } };
Light light2 = { { -sqrt(3.0), 1.0, -1.0, 0.0 } };

int xOrig = 0, yOrig = 0;
int mButton = 0, mButtonState = 0;

enum { MODE_WORLDVIEW, MODE_SELECTION } gui_mode = MODE_WORLDVIEW;

char	fps_str[256]	= " ";
long	fps_count		= 0;
double	fps_time		= 0.0;



////////////////////////////////////////////////////////////////
//
//	GUIMan_SelectVertex()
//
//		Selects the vertex under the mouse
//
void GUIMan_SelectVertex(int x, int y)
{
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLint realy;			/*  OpenGL y coordinate position  */
	GLdouble w0x, w0y, w0z;	/*  returned world x, y, z coords  */
	GLdouble w1x, w1y, w1z;
	GLdouble ox, oy, oz;


    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

	//  note viewport[3] is height of window in pixels
 /* realy = viewport[3] - (GLint) y - 1;
    printf ("Coordinates at cursor are (%4d, %4d)\n", x, realy);
    gluUnProject ((GLdouble) x, (GLdouble) realy, 0.0, 
                  mvmatrix, projmatrix, viewport, &w0x, &w0y, &w0z); 
    printf ("World coords at z=0.0 are (%f, %f, %f)\n", w0x, w0y, w0z);
    gluUnProject ((GLdouble) x, (GLdouble) realy, 1.0, 
               mvmatrix, projmatrix, viewport, &w1x, &w1y, &w1z); 
    printf ("World coords at z=1.0 are (%f, %f, %f)\n", w1x, w1y, w1z);

	Vector u(w1x-w0x, w1y-w0y, w1z-w0z);
*/

	// NOTE:	Here we project every vertex to the screen seperately and 
	//			check if it falls with a radius of r pixels of the mouse.
	//			This is the simplest but slowest way to do the selection.
	
	float minz = FLT_MAX;
	int vid = -1;

	y = viewport[3] - (GLint) y - 1;

/*	for(int k = 0; k < Sim->num_object; k++) {
		display = &(Sim->object[k]->display;
		for(int i = 0; i < display->dA_size; i += DISPARRAY_NODESIZE) {
			ox = Sim->dispArray[i+DISPARRAY_VERTEXOFFSET];
			oy = Sim->dispArray[i+DISPARRAY_VERTEXOFFSET+1];
			oz = Sim->dispArray[i+DISPARRAY_VERTEXOFFSET+2];

			gluProject ((GLdouble) ox, (GLdouble) oy, (GLdouble) oz, 
		              mvmatrix, projmatrix, viewport, &w0x, &w0y, &w0z);
		
			if ((abs(w0x-x) <= 10) && (abs(w0y-y) <= 10)) {
				if(oz < minz) {
					minz = oz;
					vid = i/DISPARRAY_NODESIZE;
				}
			}
	}
*/	
//	Vector endpoint(x, y, 0);

//	Sim->CreateSelection(vid);

	printf("Selection: vertex #%d is selected at mouse(%d, %d)\n",vid, x ,y);

}



////////////////////////////////////////////////////////////////
//
//	GUIMan_RotateScene()
//
//		Calculates the new rotation angles
//
void GUIMan_RotateScene(int newx, int newy)
{

	camera.xAngle = 180 * (newy - yOrig)/win_height + camera.prev_xAngle;
	camera.yAngle = 180 * (newx - xOrig)/win_width + camera.prev_yAngle;
  
   
	if ( camera.xAngle < 0.0 ) {
		camera.xAngle += 360.0;
	} else if ( camera.xAngle > 360.0 ) {
		camera.xAngle -= 360.0;
	}
   
	if ( camera.yAngle < 0.0 ) {
		camera.yAngle += 360.0;
	} else if ( camera.yAngle > 360.0 ) {
		camera.yAngle -= 360.0;
	}
   
	glutPostRedisplay();

}




////////////////////////////////////////////////////////////////
//
//	GUIMan_ZoomScene()
//
//		Calculates the new zoom factor
//
void GUIMan_ZoomScene(int newx, int newy)
{
	static float dx, dy, dz;

	camera.r = 10.0 * (newy - yOrig)/win_height + camera.prev_r;

	yOrig = newy;
   
	if ( camera.r < 0.1 ) {
		camera.r = 0.1;
	} 

	dx = camera.fx - camera.x;
	dy = camera.fy - camera.y;
	dz = camera.fz - camera.z;

	camera.x = camera.fx - dx/camera.prev_r*camera.r;
	camera.y = camera.fy - dy/camera.prev_r*camera.r;
	camera.z = camera.fz - dz/camera.prev_r*camera.r;

	glutPostRedisplay();
}



////////////////////////////////////////////////////////////////
//
//	GUIMan_DrawAxes()
//
//		Draws the coordinate axes
//
void GUIMan_DrawAxes( float scale )
{
	glDisable( GL_LIGHTING );

	glPushMatrix();
		glScalef( scale, scale, scale );

		glBegin( GL_LINES ); 
			glColor3f( 1.0, 0.0, 0.0 );
			// Letter X
			glVertex3f( 0.8f, 0.05f, 0.0 );	glVertex3f( 1.0, 0.25f, 0.0 ); 
			glVertex3f( 0.8f, 0.25f, 0.0 );	glVertex3f( 1.0, 0.05f, 0.0 );
			// X-Axis
			glVertex3f( 0.0, 0.0, 0.0 );
			glVertex3f( 1.0, 0.0, 0.0 ); 

			glColor3f( 0.0, 1.0, 0.0 );
			// Letter Y
			glVertex3f( 0.05, 1.0, 0.0 );	glVertex3f( 0.15, 0.9, 0.0 );
			glVertex3f( 0.15, 0.9, 0.0 );	glVertex3f( 0.25, 1.0, 0.0 );
			glVertex3f( 0.15, 0.9, 0.0 );	glVertex3f( 0.15, 0.8, 0.0 );
			// Y-Axis
			glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 1.0, 0.0 ); 

			glColor3f( 0.0, 0.0, 1.0 );
			// Letter Z
			glVertex3f( 0.0, 0.25, 1.0 );	glVertex3f( 0.0, 0.25, 0.8 ); 
			glVertex3f( 0.0, 0.25, 0.8 );	glVertex3f( 0.0, 0.05, 1.0 );
			glVertex3f( 0.0, 0.05, 1.0 );	glVertex3f( 0.0, 0.05, 0.8 );
			// Z-Axis
			glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 0.0, 1.0 ); 
		glEnd();

	glPopMatrix();

//	glEnable( GL_LIGHTING );
}



////////////////////////////////////////////////////////////////
//
//	GUIMan_DrawFloor()
//
//		Draws the floor
//
void GUIMan_DrawFloor (float size, int lineno)
{
	int i, l = lineno;
	float s = size/2;
	float p = size/l;

	glColor3f(0.6, 0.6, 0.6);
	glBegin(GL_LINE_LOOP);
		glNormal3f(0, 1, 0);
		glVertex3f( s, 0,  s);
		glVertex3f( s, 0, -s);
		glVertex3f(-s, 0, -s);
		glVertex3f(-s, 0,  s);
	glEnd();
  
	glBegin(GL_LINES);
		for(i = 0; i < l; i++) {
			glNormal3f(0, 1, 0);
			glVertex3f( s, 0, s-i*p);
			glVertex3f(-s, 0, s-i*p);
     
			glNormal3f(0, 1, 0);
			glVertex3f(s-i*p, 0,  s);
			glVertex3f(s-i*p, 0, -s);
		}
	glEnd();
}




////////////////////////////////////////////////////////////////
//
//	GUIMan_DrawString()
//
//		Draws a string
//
void GUIMan_DrawString (char *str, float x, float y)
{
	glDisable( GL_LIGHTING );

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		glOrtho ( 0.0, win_width, 0.0, win_height, -1.0, 1.0 );

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();

			glColor3f(1.0, 0.2, 0.2);
			glRasterPos2f(x, y);
			void *font = GLUT_BITMAP_HELVETICA_12;
			
			for(int i = 0; str[i]; i++)
				glutBitmapCharacter(font, str[i]);

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

//	glEnable( GL_LIGHTING );
}




////////////////////////////////////////////////////////////////
//
//	GUIMan_DrawDispArray()
//
//		Draws the display array
//
void GUIMan_DrawDispArray (void)
{    
	static int				i;
	static DisplayArray		*display;


	for(i = 0; i < Sim->num_object; i++) {
		display = Sim->object[i]->displayMngr->GetDisplay();

		if (display != NULL) {
			switch(display->header.polyMode) {
				case GIPSI_POLYGON_OUTLINE:
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					break;
				case GIPSI_POLYGON_FILL:
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					switch(display->header.shadeMode) {
						case GIPSI_SHADE_FLAT:
							glShadeModel(GL_FLAT);
							break;
						case GIPSI_SHADE_SMOOTH:
							glShadeModel(GL_SMOOTH);
							break;
					}
					break;
			}

			switch(display->header.dataType) {
				case 0x00:
					glInterleavedArrays(GL_V3F, 0, display->dispArray);
					break;
				case 0x01:
					glInterleavedArrays(GL_C3F_V3F, 0, display->dispArray);
					break;
				case 0x02:
					error_display(-1, "GUI: Unsupported data type!\n");
					break;
				case 0x04:
					glInterleavedArrays(GL_N3F_V3F, 0, display->dispArray);
					break;
				case 0x05:
					error_display(-1, "GUI: Unsupported data type!\n");
					break;
				case 0x06:
					glInterleavedArrays(GL_C4F_N3F_V3F, 0, display->dispArray);
					break;
				case 0x08:
					glInterleavedArrays(GL_T2F_V3F, 0, display->dispArray);
					break;
				case 0x09:
					glInterleavedArrays(GL_T2F_C3F_V3F, 0, display->dispArray);
					break;
				case 0x0a:
					error_display(-1, "GUI: Unsupported data type!\n");
					break;
				case 0x0c:
					glInterleavedArrays(GL_T2F_N3F_V3F, 0, display->dispArray);
					break;
				case 0x0d:
					error_display(-1, "GUI: Unsupported data type!\n");
					break;
				case 0x0e:
					glInterleavedArrays(GL_T2F_C4F_N3F_V3F, 0, display->dispArray);
					break;
			}

			switch(display->header.objType) {
				case GIPSI_DRAW_POINT:
					glDrawElements(GL_POINTS, display->iA_size, GL_UNSIGNED_INT, display->indexArray);
					break;
				case GIPSI_DRAW_LINE:
					glDrawElements(GL_LINES, display->iA_size, GL_UNSIGNED_INT, display->indexArray);
					break;
				case GIPSI_DRAW_TRIANGLE:
					glDrawElements(GL_TRIANGLES, display->iA_size, GL_UNSIGNED_INT, display->indexArray);
					break;
				case GIPSI_DRAW_QUAD:
					glDrawElements(GL_QUADS, display->iA_size, GL_UNSIGNED_INT, display->indexArray);
					break;
				case GIPSI_DRAW_POLYGON:
					glDrawElements(GL_POLYGON, display->iA_size, GL_UNSIGNED_INT, display->indexArray);
					break;
			}
		}
	}

}




////////////////////////////////////////////////////////////////
//
//	GUIMan_Display
//
//		Main display callback function
//
void GUIMan_Display(void)
{

	start_timer(0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/****************************************/
	/*		Adjust Camera View				*/
	/****************************************/
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	// Reset modelview matrix to the identity matrix
	gluLookAt(	camera.x, camera.y, camera.z, 
				camera.fx, camera.fy, camera.fz, 
				camera.upx, camera.upy, camera.upz);
	glRotatef(camera.xAngle, 1.0, 0.0, 0.0);  // Rotate by X, Y, and Z angles
	glRotatef(camera.yAngle, 0.0, 1.0, 0.0);

	glLightfv(GL_LIGHT0, GL_POSITION, light0.pos);
	glLightfv(GL_LIGHT1, GL_POSITION, light1.pos);
	glLightfv(GL_LIGHT2, GL_POSITION, light2.pos);

	GUIMan_DrawFloor(100.0, 10);
	GUIMan_DrawAxes(1.0);

	/****************************************/
	/*		Display Objects					*/
	/****************************************/
//	pthread_mutex_lock(&Sim->displayMutex);
 
	GUIMan_DrawDispArray();
      
//	pthread_mutex_unlock(&Sim->displayMutex);

	GUIMan_DrawString(fps_str, win_width-150, win_height-20);

	glutSwapBuffers();
	glutPostRedisplay();

	fps_time += get_timer(0);
	fps_count++;
	sprintf(fps_str, "fps: %.2f ", fhz(fps_time/fps_count));


}




////////////////////////////////////////////////////////////////
//
//	GUIMan_Reshape
//
//		Reshape callback function
//
void GUIMan_Reshape(int w, int h)
{
	win_width = w;
	win_height = h;

	glViewport(0,0,w,h);
  
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(70.0, (GLdouble)w/h, 0.1, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glFlush();
}



////////////////////////////////////////////////////////////////
//
//	GUIMan_Mouse
//
//		Mouse callback function
//
void GUIMan_Mouse(int button, int state, int x, int y)
{
	static bool shift;

	if(glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
		gui_mode = MODE_SELECTION;
	} else {
		gui_mode = MODE_WORLDVIEW;
	}
	
	mButton = button;
	mButtonState = state;

	switch(state) {
	case GLUT_UP:
		camera.prev_r = camera.r;
		camera.prev_xAngle = camera.xAngle;
		camera.prev_yAngle = camera.yAngle;

		if(gui_mode == MODE_SELECTION) {
//			Sim->DeleteSelection();
		}

		break;
	case GLUT_DOWN:
		xOrig = x;
		yOrig = y;

		if(gui_mode == MODE_SELECTION) {
			GUIMan_SelectVertex(x, y);
		}

		break;
	}
}




////////////////////////////////////////////////////////////////
//
//	GUIMan_Motion
//
//		Mouse motion callback function
//
void GUIMan_Motion(int x, int y)
{

	switch(mButton) {
	case GLUT_LEFT_BUTTON:		
			if (gui_mode == MODE_WORLDVIEW)
				GUIMan_RotateScene(x, y);		// Rotate
				
		break;
	case GLUT_MIDDLE_BUTTON:	// Zoom In/Out
			if (gui_mode == MODE_WORLDVIEW)
				GUIMan_ZoomScene(x, y);

		break;
	case GLUT_RIGHT_BUTTON:		// Pan


		break;
	}
}



////////////////////////////////////////////////////////////////
//
//	GUIMan_Keyboard
//
//		Keyboard callback function
//
void GUIMan_Keyboard(unsigned char key, int x, int y)
{

	switch(key) {
	case 'r':
	case 'R':
			if(Sim->IsRunning())  
				Sim->Stop();
			else
				Sim->Run();
		break;
	default:
		printf("Unknown key command!\n");
	}
}


////////////////////////////////////////////////////////////////
//
//	GUIMan_Init
//
//		Initializes the user interface
//
void GUIMan_Init(SimulationKernel *s, int width, int height, int xpos, int ypos)
{	
	win_width = width;
	win_height = height;
	win_xpos = xpos;
	win_ypos = ypos;

	Sim = s;
}



////////////////////////////////////////////////////////////////
//
//	GUIMan_InitGL
//
//		Initializes GL (lights, settings etc.)
//
int GUIMan_InitGL(void)
{
	
	GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_shininess[] = { 0.0 };
      
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light0.pos);
	glLightfv(GL_LIGHT1, GL_POSITION, light1.pos);
	glLightfv(GL_LIGHT2, GL_POSITION, light2.pos);

/*	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
*/
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); 


//	glEnable(GL_COLOR_MATERIAL);

	return 1;
}



////////////////////////////////////////////////////////////////
//
//	GUIMan_Start
//
//		Initializes and starts the user interface
//
int GUIMan_Start(void)
{
	   
	/****************************************/
	/*   Initialize GLUT and create window  */
	/****************************************/
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowPosition( win_xpos, win_ypos );
	glutInitWindowSize( win_width, win_height );
 
	main_window = glutCreateWindow( "TRPMSim" );
	glutDisplayFunc	( GUIMan_Display );
	glutReshapeFunc	( GUIMan_Reshape );  
	glutMouseFunc	( GUIMan_Mouse	 );
	glutMotionFunc	( GUIMan_Motion  );
	glutKeyboardFunc( GUIMan_Keyboard);

	/****************************************/
	/*   Initialize GL						*/
	/****************************************/
	GUIMan_InitGL();


	/****************************************/
	/*   Start GUI							*/
	/****************************************/
	glutMainLoop();

	return 1;
}