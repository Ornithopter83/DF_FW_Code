//
//
//

#include "Common.h"
#include "LmMotor.h"

LmMotor::LmMotor()
{
  
}

/*------------------------------------------------------------------
   INIT Setting of Servo Motor
------------------------------------------------------------------*/
void LmMotor::init()
{ 
}



/*------------------------------------------------------------------
   SET Request Angle(STRING) of Bobbin Motor
   (include Change to INT from STRING)

       msg : $DO xx-yy,A,ppp%  xx- Cat no, yy -item no, A - action no, ppp - Para
------------------------------------------------------------------*/
void LmMotor::onBldcString(String _msg)
{

}


void LmMotor::onBldc(int _dir, int _duty255)
{
	

}

void LmMotor::offBldc()
{


}


