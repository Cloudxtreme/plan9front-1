#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>

void
eresized(int)
{
	if(getwindow(display, Refnone) < 0)
		sysfatal("resize failed");
}

/* stolen from mothra */
void
screendump(char *name, int full)
{
	Image *b;
	int fd;

	fd=create(name, OWRITE|OTRUNC, 0666);
	if(fd==-1)
		sysfatal("can't create file");
	if(full){
		writeimage(fd, screen, 0);
	} else {
		if((b=allocimage(display, screen->r, screen->chan, 0, DNofill)) == nil){
			close(fd);
			sysfatal("can't allocate image");
		}
		draw(b, b->r, screen, 0, b->r.min);
		writeimage(fd, b, 0);
		freeimage(b);
	}
	close(fd);
}

void
main()
{
	Event e;
	Point last;
	int haslast;
	char file[128];
	
	haslast = 0;
	initdraw(0, 0, 0);
	einit(Emouse | Ekeyboard);
	draw(screen, screen->r, display->white, 0, ZP);
	flushimage(display, 1);
	while(1){
		switch(event(&e)){
		case Emouse:
			if(e.mouse.buttons & 1){
				if(haslast)
					line(screen, last, e.mouse.xy, Enddisc, Enddisc, 5, display->black, ZP);
				else
					fillellipse(screen, e.mouse.xy, 5, 5, display->black, ZP);
				
				last = e.mouse.xy;
				haslast = 1;
				flushimage(display, 1);
			} else
				haslast = 0;
			if(e.mouse.buttons & 4){
				fillellipse(screen, e.mouse.xy, 5, 5, display->white, ZP);
				flushimage(display, 1);
			}
			break;
		case Ekeyboard:
			if(e.kbdc == 'q')
				exits(nil);
			if(e.kbdc == 'c')
				draw(screen, screen->r, display->white, 0, ZP);
			if(e.kbdc == 's'){
				snprint(file, sizeof(file), "out.bit");
				if(eenter("Save to", file, sizeof(file), &e.mouse) <= 0)
					break;
				screendump(file, 0);
			}
			break;
		}
	}
}
