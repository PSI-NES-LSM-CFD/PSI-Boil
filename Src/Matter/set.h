#ifndef SET_H
#define SET_H

///////////
//       //
//  Set  //
//       //
///////////
class Set {

  public:
    Set()            {val=-1;}

    static const Set fluid() {return Set(0);}
    static const Set solid() {return Set(1);}
    static const Set rho()   {return Set(2);}
    static const Set mu()    {return Set(3);}
    static const Set cp()    {return Set(4);}
    static const Set lambda(){return Set(5);}
    static const Set gamma() {return Set(6);}
    static const Set beta () {return Set(7);}
    static const Set sigma() {return Set(8);}

    //! Operators.
    bool operator == (const Set & o) const {return val == o.val;}

//  operator int () {return val;}

  private:
    int val;

    /* avoid implicit conversions of integer to Set */
    explicit Set(const int m) {val = m;} 
};

#endif

/*-----------------------------------------------------------------------------+
 '$Id: set.h,v 1.2 2012/03/13 09:22:30 niceno Exp $'/
+-----------------------------------------------------------------------------*/
