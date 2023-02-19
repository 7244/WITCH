#include _WITCH_PATH(PR/PR.h)

#include <xcb/xcb.h>
#include <sys/shm.h>
#include <xcb/shm.h>

xcb_screen_t *_MD_SCR_get_screen(const xcb_setup_t *setup, uint32_t screen_num){
	xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);

	while(it.rem > 0){
		if(!screen_num){
			return it.data;
		}
		screen_num--;
		xcb_screen_next(&it);
	}

	return 0;
}

typedef struct{
	xcb_connection_t *conn;
	xcb_screen_t *screen;
	xcb_shm_seg_t xcb_segment;
	uint8_t *data;

	MD_SCR_Geometry_t Geometry;
}MD_SCR_t;
sint32_t MD_SCR_open(MD_SCR_t *scr){
	scr->conn = xcb_connect(NULL, NULL);
	int err = xcb_connection_has_error(scr->conn);
	if(err){
		xcb_disconnect(scr->conn);
		return 1;
	}

	const xcb_setup_t *setup = xcb_get_setup(scr->conn);

	scr->screen = _MD_SCR_get_screen(setup, 0);
	if(!scr->screen){
		return 1;
	}

	xcb_get_geometry_cookie_t geo_cookie = xcb_get_geometry(scr->conn, scr->screen->root);
	xcb_get_geometry_reply_t *geo_reply = xcb_get_geometry_reply(scr->conn, geo_cookie, NULL);
	if(!geo_reply){
		xcb_disconnect(scr->conn);
		return 1;
	}

	uint32_t shmsize = geo_reply->width * geo_reply->height * 4;

	scr->Geometry.Resolution.x = geo_reply->width;
	scr->Geometry.Resolution.y = geo_reply->height;

	/* TODO how this backend tells LineSize? */
	scr->Geometry.LineSize = scr->Geometry.Resolution.x * 4;

	int shmid = shmget(IPC_PRIVATE, shmsize, IPC_CREAT | 0777);
	if(shmid == -1){
		xcb_disconnect(scr->conn);
		return 1;
	}

	scr->xcb_segment = xcb_generate_id(scr->conn);
	xcb_shm_attach(scr->conn, scr->xcb_segment, shmid, 0);
	scr->data = (uint8_t *)shmat(shmid, 0, 0);
	shmctl(shmid, IPC_RMID, 0);
	if((sintptr_t)scr->data == -1 || !scr->data){
		xcb_disconnect(scr->conn);
		return 1;
	}

	return 0;
}
void MD_SCR_close(MD_SCR_t *scr){
	xcb_shm_detach(scr->conn, scr->xcb_segment);
	if(shmdt(scr->data) != 0){
		PR_abort();
	}
	xcb_disconnect(scr->conn);
}

uint8_t *MD_SCR_read(MD_SCR_t *scr){
	xcb_shm_get_image_cookie_t image_cookie = xcb_shm_get_image(
		scr->conn,
		scr->screen->root,
		0,
		0,
		scr->Geometry.Resolution.x,
		scr->Geometry.Resolution.y,
		~0,
		XCB_IMAGE_FORMAT_Z_PIXMAP,
		scr->xcb_segment,
		0);
	xcb_shm_get_image_reply_t *image_reply = xcb_shm_get_image_reply(
		scr->conn,
		image_cookie,
		NULL);
	if(image_reply == NULL){
		return NULL;
	}

	xcb_flush(scr->conn);

	free(image_reply);

	return scr->data;
}
