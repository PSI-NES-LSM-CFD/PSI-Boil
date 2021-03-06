#include "bndcnd.h"

/******************************************************************************/
BndCnd::BndCnd( const Domain & d ) 
 : typ( BndType::dirichlet() ),
   dir( Dir::undefined() ),
   dom( &d ),
   ir(0,-1), jr(0,-1), kr(0,-1) {

  val[0]=0.0; 
  val[1]=0.0; 
  val[2]=0.0;
}

/******************************************************************************/
BndCnd::BndCnd(const Dir & dr, const BndType & bt, const BndVal & u, 
                                                   const BndVal & v, 
                                                   const BndVal & w) 
/*----------------------------------------------------------------+
|  creates a temprorary object when sending a parameter to "add"  |
+----------------------------------------------------------------*/
 : dir(dr), 
   typ(bt), 
   ir(0,-1), jr(0,-1), kr(0,-1) {
  val[0] = u; 
  val[1] = v; 
  val[2] = w;
}

/******************************************************************************/
BndCnd::BndCnd( const Dir        & dr, 
                const Range<int> & j_r, 
                const Range<int> & k_r, const BndType & bt, const BndVal & u,
                                                            const BndVal & v, 
                                                            const BndVal & w ) 
/*----------------------------------------------------------------+
|  creates a temprorary object when sending a parameter to "add"  |
+----------------------------------------------------------------*/
 : dir(dr), 
   typ(bt),
   ir(0,-1), jr(j_r), kr(k_r) {
 
  assert( dr == Dir::imin() || dr == Dir::imax() );

  val[0] = u; 
  val[1] = v; 
  val[2] = w;
}

/******************************************************************************/
BndCnd::BndCnd( const Range<int> & i_r,
                const Dir        & dr,  
                const Range<int> & k_r, const BndType & bt, const BndVal & u,
                                                            const BndVal & v, 
                                                            const BndVal & w ) 
/*----------------------------------------------------------------+
|  creates a temprorary object when sending a parameter to "add"  |
+----------------------------------------------------------------*/
 : dir(dr), 
   typ(bt),
   ir(i_r), jr(0,-1), kr(k_r) {
 
  assert( dr == Dir::jmin() || dr == Dir::jmax() );

  val[0] = u; 
  val[1] = v; 
  val[2] = w;
}

/******************************************************************************/
BndCnd::BndCnd( const Range<int> & i_r,
                const Range<int> & j_r, 
                const Dir        & dr,  const BndType & bt, const BndVal & u,
                                                            const BndVal & v, 
                                                            const BndVal & w ) 
/*----------------------------------------------------------------+
|  creates a temprorary object when sending a parameter to "add"  |
+----------------------------------------------------------------*/
 : dir(dr), 
   typ(bt),
   ir(i_r), jr(j_r), kr(0,-1) {
 
  assert( dr == Dir::kmin() || dr == Dir::kmax() );

  val[0] = u; 
  val[1] = v; 
  val[2] = w;
}

/******************************************************************************/
BndCnd::BndCnd( const Range<int> & i_r,
                const Range<int> & j_r, 
                const Range<int> & k_r, 
                const Dir        & dr,  const BndType & bt, const BndVal & u,
                                                            const BndVal & v, 
                                                            const BndVal & w ) 
/*----------------------------------------------------------------+
|  creates a temprorary object when sending a parameter to "add"  |
+----------------------------------------------------------------*/
 : dir(dr), 
   typ(bt),
   ir(i_r), jr(j_r), kr(k_r) {
 
  assert( dr == Dir::ibody() );

  val[0] = u; 
  val[1] = v; 
  val[2] = w;
}

/******************************************************************************/
void BndCnd::modify(BndCnd bc) {
  for(int b=0; b<count(); b++){
    if(section.at(b).typ == bc.typ && section.at(b).dir == bc.dir){
      section.at(b).val[0] = bc.val[0];
      section.at(b).val[1] = bc.val[1];
      section.at(b).val[2] = bc.val[2];
    }
  }
}

/******************************************************************************/
void BndCnd::add(BndCnd bc) {

  bool fail;

  /*-------------------------------------------------------------+
  |  1. check consistency of (non) periodic boundary conditions  | 
  +-------------------------------------------------------------*/
  fail = false;
  if( bc.typ == BndType::periodic() ) {
    if( bc.dir == Dir::imin() && !dom->period(0) ) fail = true;
    if( bc.dir == Dir::imax() && !dom->period(0) ) fail = true;
    if( bc.dir == Dir::jmin() && !dom->period(1) ) fail = true;
    if( bc.dir == Dir::jmax() && !dom->period(1) ) fail = true;
    if( bc.dir == Dir::kmin() && !dom->period(2) ) fail = true;
    if( bc.dir == Dir::kmax() && !dom->period(2) ) fail = true;
  }
  if( fail ) {
    boil::oout << "Fatal: trying to define periodic b.c. " 
               << "to a non-periodic domain. Exiting!" << boil::endl;
    exit(0);
  }

  fail = false;
  if( bc.typ != BndType::periodic() ) {
    if( bc.dir == Dir::imin() && dom->period(0) ) fail = true;
    if( bc.dir == Dir::imax() && dom->period(0) ) fail = true;
    if( bc.dir == Dir::jmin() && dom->period(1) ) fail = true;
    if( bc.dir == Dir::jmax() && dom->period(1) ) fail = true;
    if( bc.dir == Dir::kmin() && dom->period(2) ) fail = true;
    if( bc.dir == Dir::kmax() && dom->period(2) ) fail = true;
  }
  if( fail ) {
    boil::oout << "Fatal: trying to define non-periodic b.c. " 
               << "to a periodic domain. Exiting!" << boil::endl;
    exit(0);
  }

  /*------------------------------------------------------------+
  |  1. skip this b.c. if it does not belong to this subdomain  | 
  +------------------------------------------------------------*/
  fail = false; 

  if(bc.dir == Dir::imin() && dom->coord(Comp::i()) != 0)             
    fail = true;
  if(bc.dir == Dir::imax() && dom->coord(Comp::i()) != dom->dim(Comp::i())-1) 
    fail = true;

  if(bc.dir == Dir::jmin() && dom->coord(Comp::j()) != 0)             
    fail = true;
  if(bc.dir == Dir::jmax() && dom->coord(Comp::j()) != dom->dim(Comp::j())-1) 
    fail = true;

  if(bc.dir == Dir::kmin() && dom->coord(Comp::k()) != 0)             
    fail = true;
  if(bc.dir == Dir::kmax() && dom->coord(Comp::k()) != dom->dim(Comp::k())-1) 
    fail = true;

  if( fail ) {
    bc.ir.first( 0); bc.jr.first( 0); bc.kr.first( 0);
    bc.ir.last (-1); bc.jr.last (-1); bc.kr.last (-1);
    section.push_back(bc);
    return;
  }

  /*---------------------------------------------------+
  |  2. set range for directions explicitelly defined  | 
  +---------------------------------------------------*/
  if(bc.dir == Dir::imin()) {bc.ir.first(0);
                             bc.ir.last (0);}
  if(bc.dir == Dir::imax()) {bc.ir.first(dom->ni()-1);
                             bc.ir.last (dom->ni()-1);}

  if(bc.dir == Dir::jmin()) {bc.jr.first(0);
                             bc.jr.last (0);}
  if(bc.dir == Dir::jmax()) {bc.jr.first(dom->nj()-1);
                             bc.jr.last (dom->nj()-1);}

  if(bc.dir == Dir::kmin()) {bc.kr.first(0);
                             bc.kr.last (0);}
  if(bc.dir == Dir::kmax()) {bc.kr.first(dom->nk()-1);
                             bc.kr.last (dom->nk()-1);}

  /*--------------------------------------+
  |  3. set the range for the directions  |
  |         not explicitelly defined      |
  +--------------------------------------*/
  Range<int> cxg = dom->cxg(); 
  Range<int> cyg = dom->cyg(); 
  Range<int> czg = dom->czg(); 

  if( bc.ir.first() == 0 && bc.ir.last() == -1 ) {
    bc.ir.first( cxg.first() ); // as sx()
    bc.ir.last ( cxg.last()  ); // as ex()
  }
  if( bc.jr.first() == 0 && bc.jr.last() == -1 ) {
    bc.jr.first( cyg.first() ); // as sy()
    bc.jr.last ( cyg.last()  ); // as ey()
  }
  if( bc.kr.first() == 0 && bc.kr.last() == -1 ) {
    bc.kr.first( czg.first() ); // as sz()
    bc.kr.last ( czg.last()  ); // as ez()
  }

  /*---------------------------------------------------+
  |  4. correct the range for directions defined by 3  |
  +---------------------------------------------------*/
  bool i = bc.dir==Dir::imin() || bc.dir==Dir::imax(), 
       j = bc.dir==Dir::jmin() || bc.dir==Dir::jmax(),
       k = bc.dir==Dir::kmin() || bc.dir==Dir::kmax();
  bool b = bc.dir==Dir::ibody();                              

  if( j || k || b ) {

    /* if both outside the range - skip this b.c. */
    if( bc.ir.first() < cxg.first() && bc.ir.last() < cxg.first() ||
        bc.ir.first() > cxg.last() && bc.ir.last() > cxg.last() ) {
      bc.ir.first( 0); bc.jr.first( 0); bc.kr.first( 0);
      bc.ir.last (-1); bc.jr.last (-1); bc.kr.last (-1);
    }
    else {
      if(cxg.contains(bc.ir.first())) bc.ir.first(bc.ir.first()-cxg.first()+1);
      else                          bc.ir.first(1);
      if(cxg.contains(bc.ir.last())) bc.ir.last(bc.ir.last()-cxg.first()+1);
      else                         bc.ir.last(dom->ni()-2);
    }
  } /* j || k || b */

  if( i || k || b ) {

    /* if both outside the range - skip this b.c. */
    if( bc.jr.first() < cyg.first() && bc.jr.last() < cyg.first() ||
        bc.jr.first() > cyg.last() && bc.jr.last() > cyg.last() ) {
      bc.ir.first( 0); bc.jr.first( 0); bc.kr.first( 0);
      bc.ir.last (-1); bc.jr.last (-1); bc.kr.last (-1);
    }
    else {
      if(cyg.contains(bc.jr.first())) bc.jr.first(bc.jr.first()-cyg.first()+1);
      else                          bc.jr.first(1);
      if(cyg.contains(bc.jr.last())) bc.jr.last(bc.jr.last()-cyg.first()+1);
      else                         bc.jr.last(dom->nj()-2);
    }
  } /* i || k || b */

  if( i || j || b ) {

    /* if both outside the range - skip this b.c. */
    if( bc.kr.first() < czg.first() && bc.kr.last() < czg.first() ||
        bc.kr.first() > czg.last() && bc.kr.last() > czg.last() ) {
      bc.ir.first( 0); bc.jr.first( 0); bc.kr.first( 0);
      bc.ir.last (-1); bc.jr.last (-1); bc.kr.last (-1);
    }
    else {
      if(czg.contains(bc.kr.first())) bc.kr.first(bc.kr.first()-czg.first()+1);
      else                          bc.kr.first(1);
      if(czg.contains(bc.kr.last())) bc.kr.last(bc.kr.last()-czg.first()+1);
      else                         bc.kr.last(dom->nk()-2);
    }
  } /* i || j || b */

  section.push_back(bc);
}

/******************************************************************************/
BndType & BndCnd::type(const int b) {
  return section.at(b).typ;
}

/******************************************************************************/
char * BndCnd::formula(const int b, const Comp m) const {
  return section.at(b).val[~m].c;
}

/******************************************************************************/
real BndCnd::value(const int b, const Comp m) const {
  return section.at(b).val[~m].r;
}

/******************************************************************************/
real BndCnd::value(const int b, const real x, const real y, const real z, 
                   const Comp m) const {

  /* if formula is not defined */
  if(!section.at(b).val[~m].c)
    return section.at(b).val[~m].r;

  /* if formula is defined */
  else {
    Formula f;

    std::stringstream xs, ys, zs;
    xs << "x=" << x; f.evaluate(xs);
    ys << "y=" << y; f.evaluate(ys);
    zs << "z=" << z; f.evaluate(zs);

    return f.evaluate(section.at(b).val[~m].c);
  }
}

/******************************************************************************/
int BndCnd::count() const {
  return section.size();
}

/******************************************************************************/
const Dir & BndCnd::direction(const int b) const {
  return section.at(b).dir;
}

/******************************************************************************/
bool BndCnd::type(const Dir & dir, const BndType & bt) const {

  for(int b=0; b<count(); b++) 
    if( section.at(b).typ == bt &&              
        section.at(b).dir == dir ) return true;

  return false;
}

/******************************************************************************/
int BndCnd::index(const Dir & dir, const BndType & bt) const {

  for(int b=0; b<count(); b++){
    if( section.at(b).typ == bt &&
        section.at(b).dir == dir ) return b;
  }
  return -1;
}

/******************************************************************************/
bool BndCnd::type_here(const Dir & dir, const BndType & bt) const {

  if( dir == Dir::imin() && dom->coord(Comp::i()) != 0 )             
    return false;
  if( dir == Dir::imax() && dom->coord(Comp::i()) != dom->dim(Comp::i())-1 ) 
    return false;
  if( dir == Dir::jmin() && dom->coord(Comp::j()) != 0 )             
    return false;
  if( dir == Dir::jmax() && dom->coord(Comp::j()) != dom->dim(Comp::j())-1 ) 
    return false;
  if( dir == Dir::kmin() && dom->coord(Comp::k()) != 0 )             
    return false;
  if( dir == Dir::kmax() && dom->coord(Comp::k()) != dom->dim(Comp::k())-1 ) 
    return false;

  for(int b=0; b<count(); b++) 
    if( section.at(b).typ == bt &&              
        section.at(b).dir == dir ) return true;

  return false;
}

/******************************************************************************/
bool BndCnd::type_decomp(const int b) const {
  
  Dir dir = section.at(b).dir;
  
  if( dir == Dir::imin() && dom->coord(Comp::i()) == 0 )
    return false;
  if( dir == Dir::imax() && dom->coord(Comp::i()) == dom->dim(Comp::i())-1 )
    return false;
  if( dir == Dir::jmin() && dom->coord(Comp::j()) == 0 )
    return false;
  if( dir == Dir::jmax() && dom->coord(Comp::j()) == dom->dim(Comp::j())-1 )
    return false;
  if( dir == Dir::kmin() && dom->coord(Comp::k()) == 0 )
    return false;
  if( dir == Dir::kmax() && dom->coord(Comp::k()) == dom->dim(Comp::k())-1 )
    return false;
  
  return true;
}

/******************************************************************************/
bool BndCnd::type_decomp(const Dir dir) const {

  if( dir == Dir::imin() && dom->coord(Comp::i()) == 0 )
    return false;
  if( dir == Dir::imax() && dom->coord(Comp::i()) == dom->dim(Comp::i())-1 )
    return false;
  if( dir == Dir::jmin() && dom->coord(Comp::j()) == 0 )
    return false;
  if( dir == Dir::jmax() && dom->coord(Comp::j()) == dom->dim(Comp::j())-1 )
    return false;
  if( dir == Dir::kmin() && dom->coord(Comp::k()) == 0 )
    return false;
  if( dir == Dir::kmax() && dom->coord(Comp::k()) == dom->dim(Comp::k())-1 )
    return false;

  return true;
}

/******************************************************************************/
bool BndCnd::exists(const BndType & bt) const {

  int here=0;

  /* look for bt among boundary conditions */
  for(int b=0; b<count(); b++) {
    if( section.at(b).typ == bt ) {here=1; break;}
  }

  /* check if it is found on other processor(s) */
  boil::cart.sum_int(&here);

  if(here) 
    return true;

  return false;
}

/******************************************************************************/
bool BndCnd::exists(const int b) const {
  return section.at(b).ir.exists() &&
         section.at(b).jr.exists() &&
         section.at(b).kr.exists();
}

/***************************************************************************//**
*  Prints the name of the current booundary condition. 
*  Needed only for information or debugging.
*******************************************************************************/
std::ostream & operator << (std::ostream &ost, const BndCnd & bc) {

  ost << std::endl;
  ost << "type: " << bc.typ << std::endl;
  ost << "dir:  " << bc.dir << std::endl;
  ost << "i: "    << bc.ir  << std::endl;
  ost << "j: "    << bc.jr  << std::endl;
  ost << "k: "    << bc.kr;

  return ost;
}

/***************************************************************************//**
*  Prints the name of the current booundary type. 
*  Needed only for information or debugging.
*******************************************************************************/
std::ostream & operator << (std::ostream &ost, const BndType & bt) {

  switch(bt.val) {
    case( 0): ost << "undefined ";  break;
    case( 1): ost << "dirichlet ";  break;
    case( 2): ost << "neumann ";    break;
    case( 3): ost << "periodic ";   break;
    case( 4): ost << "inlet ";      break;
    case( 5): ost << "outlet ";     break;
    case( 6): ost << "wall ";       break;
    case( 7): ost << "symmetry ";   break;
    case( 8): ost << "insert ";     break;
    case( 9): ost << "convective "; break;
  }
    
  return ost;
}

/*-----------------------------------------------------------------------------+
 '$Id: bndcnd.cpp,v 1.24 2016/02/26 11:07:39 niceno Exp $'/
+-----------------------------------------------------------------------------*/
