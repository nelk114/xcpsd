#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlocale.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysymdef.h>
#include <X11/extensions/XTest.h>
#define NE XNextEvent(d,&e)
#define itr(b,c) for(b;c;b)
#define W(s) write(2,s "\n",sizeof(s))
int main(int C,char**V){
setlocale(LC_ALL,"");
Display*d=XOpenDisplay(getenv("DISPLAY"));if(!d){W("!!$DISPLAY");exit(111);}
{int _;if(!XTestQueryExtension(d,&_,&_,&_,&_)){W("!!XTest");exit(111);}}
Window w=RootWindow(d,DefaultScreen(d));if(!w){W("!!RootWindow");exit(111);}
XSelectInput(d,w,KeyPressMask);XFlush(d);
XIC c=0;{
	char*M;if(!(M=XSetLocaleModifiers("@im=none")))W("!SetLocaleModifiers");/*Unclear this is necessary*/
	XIM m=XOpenIM(d,0,0,0);XIMStyles*S;XIMStyle s;if(!m){W("!!XIM");exit(111);}
	if((M=XGetIMValues(m,XNQueryInputStyle,&S,0))||!S)W("!XIMStyle");if(S){
		s=0;int i=0;for(;i<S->count_styles;i++)if((s=S->supported_styles[i])==(XIMPreeditNothing|XIMStatusNothing))break;
		if(i==S->count_styles)W("!XIMStyle");XFree(S);
		}
	if(m&&s&&!(c=XCreateIC(m,XNInputStyle,s,XNClientWindow,w,XNFocusWindow,w,0))){W("!!XIC");exit(111);}
	}
KeyCode m=XKeysymToKeycode(d,XK_Multi_key),t;if(!m){W("!!Multi_key");exit(111);}{
	int M,m,n;XDisplayKeycodes(d,&m,&M);KeySym*K=XGetKeyboardMapping(d,m,M-m+1,&n);
	int i=m;for(;i<M;i++)if(!K[(i-m)*n]){t=i;break;}XFree(K);if(i==M){W("!!KS");exit(111);}
	}
for(int i;i<256;i++){KeySym k;int M;XkbLookupKeySym(d,m,i,&M,&k);if(k==XK_Multi_key)XGrabKey(d,m,i,w,1,GrabModeAsync,GrabModeAsync);}
XEvent e;XKeyEvent*E=(XKeyEvent*)&e;KeySym k;int S;char _[4];
if(C>1){write(1,"\n",1);close(1);}itr(NE,){if(e.type!=KeyPress)continue;
	XmbLookupString(c,E,_,4,&k,&S);XFilterEvent(&e,0);if(k==XK_Multi_key){
		{Window f;int F;XGetInputFocus(d,&f,&F);XGrabKeyboard(d,w,1,GrabModeAsync,GrabModeAsync,0);XSetInputFocus(d,f,F,0);}
		itr(NE,){if(e.type!=KeyPress)continue;
			if(XmbLookupString(c,E,_,4,&k,&S)&&!XFilterEvent(&e,0)){
				XUngrabKeyboard(d,0);
				XChangeKeyboardMapping(d,t,1,&k,1);XSync(d,0);
				XTestFakeKeyEvent(d,t,1,0);XTestFakeKeyEvent(d,t,0,0);
				XGrabKeyboard(d,w,1,GrabModeAsync,GrabModeAsync,0);
				XSelectInput(d,w,KeyReleaseMask);XFlush(d);itr(NE,e.type!=KeyRelease);XSelectInput(d,w,KeyPressMask);XFlush(d);
				XUngrabKeyboard(d,0);
				k=0;XChangeKeyboardMapping(d,t,1,&k,1);XSync(d,0);
				break;
}	}	}	}	}
