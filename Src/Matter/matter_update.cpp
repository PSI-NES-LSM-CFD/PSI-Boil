#include "matter.h"

/*============================================================================*/
void Matter::look_up(const Set & s, const Scalar & sca,
                    const LookUpTable & tab,
                    const Column & col0, const Column & col1) {
/*--------------------------------------+
|  set certain property to be variable  |
+--------------------------------------*/
  if(s == Set::rho   ()) dens->look_up(sca,tab,col0,col1);
  if(s == Set::mu    ()) visc->look_up(sca,tab,col0,col1);
  if(s == Set::cp    ()) capa->look_up(sca,tab,col0,col1);
  if(s == Set::lambda()) cond->look_up(sca,tab,col0,col1);
  if(s == Set::gamma ()) diff->look_up(sca,tab,col0,col1);
  if(s == Set::beta  ()) texp->look_up(sca,tab,col0,col1);
  if(s == Set::sigma ()) 
    if(tens == NULL) {
      boil::oout << "# Fatal: using surface tension makes ";
      boil::oout << "sense only for mixtures. Exiting!"; 
      boil::oout << boil::endl;    
    } else
      tens->look_up(sca,tab,col0,col1);
}

/*-----------------------------------------------------------------------------+
 '$Id: matter_update.cpp,v 1.4 2012/03/13 09:22:30 niceno Exp $'/
+-----------------------------------------------------------------------------*/
