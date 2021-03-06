#ifndef CENTERED_H
#define CENTERED_H

#include "../../Parallel/mpi_macros.h"
#include <cmath>
#include <sstream>
#include<iostream>

#include "../equation.h"
#include "../../Matrix/matrix.h"
#include "../../Global/global_minmax.h"
#include "../../Timer/timer.h"
#include "../../Plot/plot.h"   

/***************************************************************************//**
*  \brief General cell-centered Equation conservation equation.
*
*  This class discretizes and solves conservation equation for general
*  cell-centered variable \f$ \phi \f$ in integral form:
*  \f[
*        \int_V \frac{\partial \rho B_{\phi} \phi}{\partial t} dV
*      + \int_S \rho B_{\phi} {\bf u} \phi \, dS
*      = \int_S \Gamma_{\phi} \nabla \phi \, dS
*      + \int_V f_{ext} dV
*  \f] 
*  where \f$\rho\f$ is density, \f$B_{\phi}\f$ innertial coefficient
*  associated with variable \f$\phi\f$, \f$ {t} \f$ is time, \f${\bf u}\f$
*  is convective velocity, \f$\Gamma\f$ is diffusivity of variable \f$\phi\f$ 
*  and \f$f_{ext}\f$ is external source. 
*
*  After the discretization, this equation reduces to linear system of
*  equations:
*  \f[
*      [A] \cdot \{ \phi \}^N = \{ f \}
*  \f] 
*  which can be solved with either one of the Krylov sub-space family of 
*  solvers (CG, CGS, BiCGS), or the Additive Correction (AC) multigrid.
*  The superscript \f$ N \f$ in the above equation denotes the new time
*  step. All the terms without a superscript are time-averaged over 
*  the discrete time step. Right hand side \f$ \{ f \} \f$ contains parts
*  of innertial, diffusive, convective time step (depending on the time
*  stepping scheme being used), in addition to external source. For example,
*  if convection was discretized with Adams-Bashforth scheme and diffusion
*  with Crank-Nicolson, in absence of external sources, \f$ \{ f \} \f$
   would be:
*  \f[
*      \{ f \} = \{I\}^{N-1} 
               + \frac{1}{2} \{D\}^{N-1} 
               + \frac{3}{2} \{C\}^{N-1} - \frac{1}{2} \{C\}^{N-2}
*  \f] 
*  
*  This class is also a parent to specializes implementation of cell centered 
*  variables such as pressure, enthaply, concentration, etc.
*
*******************************************************************************/

////////////////
//            //
//  Centered  //
//            //
////////////////
class Centered : public Equation {
  public:
    //! Global (external, public) constructor. 
    /*!
        \param d - domain on which the variable is defined,
        \param s - unknown variable (\f$ \phi \f$),
        \param g - extarnal source  (\f$ f_{ext} \f$),
        \param v - convection velocity (\f$ {\bf u} \f$),
        \param t - simulation (physical) time (\f$ {t} \f$),
        \param flu - holds all material properties 
                     (\f$ \rho, B_{\phi}, \Gamma_{\phi} \f$),
        \param sm - Krylov subspace solver. It acts as a solver, or as a
                    smoother for AC multirid.
    */
    Centered(const Domain * d,
             const Scalar & s, const Scalar & g, 
             const Vector * v, const Times & t, 
             Matter * flu,
             Matter * sol,
             Krylov * sm = NULL) 
     : Equation(d, &t, flu, sol, sm), phi(&s), A(phi), fext(&g), u(v), 
                    fold(*d), fnew(*d), fbnd(*d), 
                    cold(*d), cnew(*d), buff(*d), 
                    res(*d), 
                    fnr(NULL), crsr(NULL) {
                      fext=phi.shape();
                      fold=phi.shape();  fnew=phi.shape();  fbnd=phi.shape();
                      cold=phi.shape();  cnew=phi.shape();  buff=phi.shape();
                      res =phi.shape();
                    }

    ~Centered() {};
	  
    //! Cell volume.
    real dV(const int i, const int j, const int k) const 
     {return phi.dV(i,j,k);}

    void discretize(const Scalar * diff_eddy = NULL) {};

    //! Start a new time step.
    void new_time_step();

    //! Advance in time for purely explicit schemes.
    void advance();

    //! Solvers discretized system of equaions.
    void solve(const ResRat & fact, const char * name = NULL); 
    virtual real update_rhs();

    void diffusion();
    void convection();

    void save(const char *, const int = -1);
    void load(const char *, const int = -1);

    //! Pointer to coarser level of the variable.
    Centered * coarser() const {return crsr;}

    //! Defines the variable at coarser levels. (Needed for AC).
    void  coarsen();     

    const Scalar & val() const {return phi;}

  friend class AC;

  protected:
    int si() const {return phi.si();}
    int sj() const {return phi.sj();}
    int sk() const {return phi.sk();}
    int ei() const {return phi.ei();}
    int ej() const {return phi.ej();}
    int ek() const {return phi.ek();}

    //! Constructor for coarser levels. 
    Centered(const Centered * fin, const Domain *, 
             BndCnd & ubc, Krylov * sm);  

    int ni() const {return phi.ni();} 
    int nj() const {return phi.nj();} 
    int nk() const {return phi.nk();} 

    /* connection dimensions needed for discretization.
       keep i,j,k here instead of x,y,z to be more general. */
    real dxw(const int i) const {return phi.dxw(i);}
    real dxe(const int i) const {return phi.dxe(i);}
    real dys(const int i) const {return phi.dys(i);}
    real dyn(const int i) const {return phi.dyn(i);}
    real dzb(const int i) const {return phi.dzb(i);}
    real dzt(const int i) const {return phi.dzt(i);}

    /* needed for discretization.
       avoid dxc, dyc and dzc here, use dSx, dSy, dSz and dV instead. */
    real dSx(const int i, const int j, const int k) const 
     {return phi.dSx(i,j,k);}
    real dSy(const int i, const int j, const int k) const 
     {return phi.dSy(i,j,k);}
    real dSz(const int i, const int j, const int k) const 
     {return phi.dSz(i,j,k);}

    void create_system_innertial(const Property * f_prop,  
                                 const Property * s_prop = NULL);
    void create_system_diffusive(const Property * f_prop, 
                                 const Property * s_prop = NULL,
                                 const Scalar * diff_eddy = NULL);
    void create_system_bnd(const Property * f_prop = NULL);

    void convection(Scalar * conv, const Property * prop); 
    void new_time_step(const Property * f_prop,
                       const Property * s_prop = NULL);

    Scalar phi;                    // can't be inherited if protected !?!?!?
    Scalar fold, fnew, fext, fbnd; // source
    Matrix A;                      // matrix
    Scalar res;                

    Scalar cold, cnew, buff;

    const Vector * u;

    const Centered * fnr;
          Centered * crsr;
};	

#endif

/*-----------------------------------------------------------------------------+
 '$Id: centered.h,v 1.66 2016/02/26 11:07:40 niceno Exp $'/
+-----------------------------------------------------------------------------*/
