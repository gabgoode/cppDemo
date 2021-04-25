#include "dcf.h"
#include "fixdcf.h"
#include <iostream>

dcf::dcf()
{

}

//Determine whether this is a fix rate bond or a float rate bond

dcf::filterRateType(BondRef& bondref){
    //1 as fix rate, 2 as float rate
    if (bondref.rate_type == 1){
        //use fixdcf
    }else{
        //use floatdcf
    }
}

