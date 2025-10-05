#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlocale.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysymdef.h>
#include <X11/extensions/XTest.h>
#define fI(n) for(int i=0;i<n;i++)
#define fE for(;XNextEvent(d,&e),1;)
#define E(s,e) (write(2,s "\n",sizeof(s)),e<0?0:(exit(e),0))
int L(XIC c,XEvent*e,KeySym*k,int*S){char _[4];return!!XmbLookupString(c,(XKeyEvent*)e,_,4,k,S)&!XFilterEvent(e,0);}
int H(Display*d,XErrorEvent*e){e->error_code==BadAccess&&E("!!Grab",111);XSetErrorHandler(0);}
int main(int C,char**V){
setlocale(LC_ALL,"");
Display*d=XOpenDisplay(0);d||E("!!$DISPLAY",111);
{int _;XTestQueryExtension(d,&_,&_,&_,&_)||E("!!XTest",111);}
Window w=DefaultRootWindow(d);w||E("!!RootWindow",111);
XIC c=0;{
	XSetLocaleModifiers("@im=none")||E("!SetLocaleModifiers",-1);
	XIM m=XOpenIM(d,0,0,0);XIMStyles*S;XIMStyle s=0;m||E("!!XIM",111);
	!XGetIMValues(m,XNQueryInputStyle,&S,0)&&S||E("!XIMStyle",-1);if(S){
		fI(S->count_styles)if((s=S->supported_styles[i])==(XIMPreeditNothing|XIMStatusNothing))goto f;
		E("!XIMStyle",-1);f:XFree(S);
		}
	s&&!(c=XCreateIC(m,XNInputStyle,s,XNClientWindow,w,0))&&E("!!XIC",111);
	}
XSetErrorHandler(H);XSelectInput(d,w,KeyPressMask);
KeyCode m=XKeysymToKeycode(d,XK_Multi_key),t;m||E("!!Multi_key",111);{
	int M,m,n;XDisplayKeycodes(d,&m,&M);KeySym*K=XGetKeyboardMapping(d,m,M-m+1,&n);
	fI(M-m)if(!K[i*n]){t=i+m;goto k;}E("!!KS",111);k:XFree(K);
	}
fI(256){KeySym k;int _;XkbLookupKeySym(d,m,i,&_,&k);k==XK_Multi_key&&XGrabKey(d,m,i,w,1,GrabModeAsync,GrabModeAsync);}
XSync(d,0);
XEvent e;KeySym k[2];int S;if(C>1)write(1,"\n",1),close(1);fE if(e.type==KeyPress&&(L(c,&e,k,&S),*k==XK_Multi_key)){
#define v ((XKeyEvent*)&e)->time
	{
		Window f;int F;XGetInputFocus(d,&f,&F);
		if(XGrabKeyboard(d,w,1,GrabModeAsync,GrabModeAsync,v)){E("!GrabKeyboard",-1);continue;}
		XSetInputFocus(d,f,F,0);
		}
	fE if(e.type==KeyPress&&L(c,&e,k,&S)){
		k[1]=*k;XChangeKeyboardMapping(d,t,2,k,1);XSync(d,0);
		XUngrabKeyboard(d,v);XTestFakeKeyEvent(d,t,1,0);XTestFakeKeyEvent(d,t,0,0);
		if(XGrabKeyboard(d,w,1,GrabModeAsync,GrabModeAsync,v))E("!再GrabKeyboard",-1);else{
			XSelectInput(d,w,KeyReleaseMask);fE if(e.type==KeyRelease)break;XSelectInput(d,w,KeyPressMask);XFlush(d);
			XUngrabKeyboard(d,v);
			}
		*k=0;XChangeKeyboardMapping(d,t,1,k,1);XSync(d,0);
		break;
}	}	}
