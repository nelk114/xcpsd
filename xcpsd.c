#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlocale.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysymdef.h>
#include <X11/extensions/XTest.h>
#define fE(w,i) for(;XNextEvent(d,&e),w;)if(i)
#define E(s,e) write(2,s "\n",sizeof(s)),e<0?0:exit(e)
int L(XIC c,XEvent*e,KeySym*k,int*S){char _[4];return!!XmbLookupString(c,(XKeyEvent*)e,_,4,k,S)&!XFilterEvent(e,0);}
int main(int C,char**V){
setlocale(LC_ALL,"");
Display*d=XOpenDisplay(getenv("DISPLAY"));if(!d)E("!!$DISPLAY",111);
{int _;if(!XTestQueryExtension(d,&_,&_,&_,&_))E("!!XTest",111);}
Window w=RootWindow(d,DefaultScreen(d));if(!w)E("!!RootWindow",111);
XSelectInput(d,w,KeyPressMask);XFlush(d);
XIC c=0;{
	if(!XSetLocaleModifiers("@im=none"))E("!SetLocaleModifiers",-1);/*Unclear this is necessary*/
	XIM m=XOpenIM(d,0,0,0);XIMStyles*S;XIMStyle s;if(!m)E("!!XIM",111);
	if(XGetIMValues(m,XNQueryInputStyle,&S,0)||!S)E("!XIMStyle",-1);if(S){
		s=0;int i=0;for(;i<S->count_styles;i++)if((s=S->supported_styles[i])==(XIMPreeditNothing|XIMStatusNothing))break;
		if(i==S->count_styles)E("!XIMStyle",-1);XFree(S);
		}
	if(m&&s&&!(c=XCreateIC(m,XNInputStyle,s,XNClientWindow,w,XNFocusWindow,w,0)))E("!!XIC",111);
	}
KeyCode m=XKeysymToKeycode(d,XK_Multi_key),t;if(!m)E("!!Multi_key",111);{
	int M,m,n;XDisplayKeycodes(d,&m,&M);KeySym*K=XGetKeyboardMapping(d,m,M-m+1,&n);
	int i=m;for(;i<M;i++)if(!K[(i-m)*n]){t=i;break;}XFree(K);if(i==M)E("!!KS",111);
	}
for(int i;i<256;i++){KeySym k;int _;XkbLookupKeySym(d,m,i,&_,&k);if(k==XK_Multi_key)XGrabKey(d,m,i,w,1,GrabModeAsync,GrabModeAsync);}
XEvent e;KeySym k;int S;if(C>1)write(1,"\n",1),close(1);fE(1,e.type==KeyPress&&(L(c,&e,&k,&S),k==XK_Multi_key)){
	{Window f;int F;XGetInputFocus(d,&f,&F);XGrabKeyboard(d,w,1,GrabModeAsync,GrabModeAsync,0);XSetInputFocus(d,f,F,0);}
	fE(1,e.type==KeyPress&&L(c,&e,&k,&S)){
		XUngrabKeyboard(d,0);
		XChangeKeyboardMapping(d,t,1,&k,1);XSync(d,0);
		XTestFakeKeyEvent(d,t,1,0);XTestFakeKeyEvent(d,t,0,0);
		XGrabKeyboard(d,w,1,GrabModeAsync,GrabModeAsync,0);
		XSelectInput(d,w,KeyReleaseMask);XFlush(d);fE(e.type!=KeyRelease,0);XSelectInput(d,w,KeyPressMask);XFlush(d);
		XUngrabKeyboard(d,0);
		k=0;XChangeKeyboardMapping(d,t,1,&k,1);XSync(d,0);
		break;
}	}	}
