#include _WITCH_PATH(MEM/MEM.h)

#include <X11/Xlib.h>
#include <X11/Xutil.h>

sint32_t MD_SCR_Get_Resolution(MD_SCR_Resolution_t *Resolution){
	Display *display = XOpenDisplay(0);
	if(!display){
		return 1;
	}
	Window root = DefaultRootWindow(display);
	XWindowAttributes attr;
	MEM_set(0, &attr, sizeof(attr));
	if(!XGetWindowAttributes(display, root, &attr)){
		return 1;
	}
	XCloseDisplay(display);
	Resolution->x = attr.width;
	Resolution->y = attr.height;
	return 0;
}

typedef struct{
	Display *_display;
	Window _root;
	XWindowAttributes _attributes;
	XImage *_img;

	MD_SCR_Geometry_t Geometry;
}MD_SCR_t;
sint32_t MD_SCR_open(MD_SCR_t *scr){
	scr->_display = XOpenDisplay(0);
	scr->_root = DefaultRootWindow(scr->_display);
	MEM_set(0, &scr->_attributes, sizeof(scr->_attributes));
	XGetWindowAttributes(scr->_display, scr->_root, &scr->_attributes);
	scr->Geometry.Resolution.x = scr->_attributes.width;
	scr->Geometry.Resolution.y = scr->_attributes.height;

	/* TODO how this backend tells LineSize? */
	scr->Geometry.LineSize = scr->Geometry.Resolution.x * 4;

	scr->_img = 0;
	return 0;
}
void MD_SCR_close(MD_SCR_t *scr){
	if(scr->_img){
		XDestroyImage(scr->_img);
	}
	XCloseDisplay(scr->_display);
}

uint8_t *MD_SCR_read(MD_SCR_t *scr){
	if(scr->_img){
		XDestroyImage(scr->_img);
	}
	scr->_img = XGetImage(
		scr->_display,
		scr->_root,
		0,
		0,
		scr->Geometry.Resolution.x,
		scr->Geometry.Resolution.y,
		AllPlanes,
		ZPixmap);
	return (uint8_t *)scr->_img->data;
}
