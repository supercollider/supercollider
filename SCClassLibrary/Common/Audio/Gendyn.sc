//GENDYN by Iannis Xenakis implemented for SC3 by sicklincoln with some refinements

Gendy1 : UGen {

			*ar { arg ampdist=1, durdist=1, adparam=1.0, ddparam=1.0, minfreq=440, maxfreq=660, ampscale= 0.5, durscale=0.5, initCPs= 12, knum, mul=1.0,add=0.0;
    					 ^this.multiNew('audio', ampdist, durdist, adparam, ddparam, minfreq, maxfreq, ampscale, durscale, initCPs, knum ? initCPs).madd( mul, add )
   			 }

			*kr {arg ampdist=1, durdist=1, adparam=1.0, ddparam=1.0, minfreq=20, maxfreq=1000, ampscale= 0.5, durscale=0.5, initCPs= 12, knum,mul=1.0,add=0.0;
    					^this.multiNew('control', ampdist, durdist, adparam, ddparam, minfreq, maxfreq, ampscale, durscale, initCPs, knum ? initCPs).madd( mul, add )
    			 }

        }



Gendy2 : UGen {

			*ar { arg ampdist=1, durdist=1, adparam=1.0, ddparam=1.0, minfreq=440, maxfreq=660, ampscale= 0.5, durscale=0.5, initCPs= 12, knum, a=1.17, c=0.31, mul=1.0,add=0.0;
    					 ^this.multiNew('audio', ampdist, durdist, adparam, ddparam, minfreq, maxfreq, ampscale, durscale, initCPs, knum ? initCPs, a, c).madd( mul, add )
   			 }

			*kr {arg ampdist=1, durdist=1, adparam=1.0, ddparam=1.0, minfreq=20, maxfreq=1000, ampscale= 0.5, durscale=0.5, initCPs= 12, knum, a=1.17, c=0.31, mul=1.0,add=0.0;
    					^this.multiNew('control', ampdist, durdist, adparam, ddparam, minfreq, maxfreq, ampscale, durscale, initCPs, knum ? initCPs, a, c).madd( mul, add )
    			 }


        }



Gendy3 : UGen {

			*ar { arg ampdist=1, durdist=1, adparam=1.0, ddparam=1.0, freq=440, ampscale= 0.5, durscale=0.5, initCPs= 12, knum, mul=1.0,add=0.0;
    					 ^this.multiNew('audio', ampdist, durdist, adparam, ddparam, freq, ampscale, durscale, initCPs, knum ? initCPs).madd( mul, add )
   			 }

			*kr {arg ampdist=1, durdist=1, adparam=1.0, ddparam=1.0, freq=440, ampscale= 0.5, durscale=0.5, initCPs= 12, knum, mul=1.0,add=0.0;
    					^this.multiNew('control', ampdist, durdist, adparam, ddparam, freq, ampscale, durscale, initCPs, knum ? initCPs).madd( mul, add )
    			 }

        }
