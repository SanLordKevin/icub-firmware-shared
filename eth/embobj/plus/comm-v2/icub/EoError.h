/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/




// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOERROR_H_
#define _EOERROR_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoError.h
    @brief      This header file contains the error codes and error strings used by the Ethernet boards in iCub. 
    @author     marco.accame@iit.it
    @date       Nov 10 2014
**/

/** @defgroup eo_EoErrors Error codes and associated strings in ETH protocol for iCub 
    This file contains error codes and error strings which are used in the iCub ETH protocol for iCub.
    Its use is in eOmn_info_properties_t::code.
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------

// these macros describe the partition of the 32 bits used by the errorcode into a category and a number.
// so far we use 8 bits to define a category (e.g., board system) and 24 bits to define a number (e.g., missing memory).
 
#define EOERROR_CATEG_MASK      0xFF000000
#define EOERROR_VALUE_MASK      0x00FFFFFF
#define EOERROR_CATEG_SHIFT     24


#define EOERROR_CATEGORY_DUMMY  EOK_uint08dummy
#define EOERROR_VALUE_DUMMY     EOK_uint32dummy
#define EOERROR_CODE_DUMMY      EOK_uint32dummy

// this macro-function should be used for constant initialisation, for all other cases use: eoerror_code_get() function
#define EOERRORCODE(cat, val)   ((val) & EOERROR_VALUE_MASK) | (((cat) << EOERROR_CATEG_SHIFT) & EOERROR_CATEG_MASK)


// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef uint32_t eOerror_code_t
    @brief      generic type used to specify an error code. it is formed of 32 bits, of which the mbb is the category, 
                and the others are a value inside that category. We represent the bits of the category with type eOerror_category_t,
                whereas we represent the bits of the value with the enum type specific of the category.
                As an example, if the category is eoerror_category_EthBoardSystem, then the enum type for the values is eOerror_value_SYS_t
                
                We also use a generic type eOerror_value_t for generic function parameter or return value.
 **/
typedef uint32_t eOerror_code_t;


/** @typedef    typedef enum eOerror_category_t
    @brief      it contains the category in the msb in eOerror_code_t. if you want to add a new category: use CamelCaseConvention for the 
                enumerative label, add it just before eoerror_category_dummy.        
 **/
typedef enum
{
    eoerror_category_System                 = 0,    /**< errors generated by the system used inside the Ethernet board: EMS4, MC4plus, ... KEEP IT ZERO */
    eoerror_category_HardWare               = 1,    /**< errors related to hardware */
    eoerror_category_MotionControl          = 2,    /**< errors generated by motion control activity */
    // add a new category in here. remember to increment eoerror_category_numberof 
    eoerror_category_dummy                   = EOERROR_CATEGORY_DUMMY   /**< used to express a dummy category. dont count it for eoerror_category_numberof */
} eOerror_category_t;

enum { eoerror_category_numberof = 3 };


/** @typedef    typedef uint32_t eOerror_value_t
    @brief      generic value for use as a function parameter. For usage, you must prefer the enumerative type relevant to the category.         
 **/
typedef uint32_t eOerror_value_t;


/** @typedef    typedef enum eOerror_value_SYS_t
    @brief      it contains the values of the eoerror_category_System category. In case you need a new value: add it
                in consecutive mode without holes and increment eoerror_value_SYS_numberof. In case you need to remove a value, dont
                remove the number but simply rename it: that will help detecting old boards sending pairs {category, value} not supported yet.  
 **/
typedef enum
{
    eoerror_value_SYS_unspecified                   = 0,
    eoerror_value_SYS_tobedecided                   = 1,
    eoerror_value_SYS_memory_zerorequested          = 2,
    eoerror_value_SYS_memory_notinitialised         = 3,
    eoerror_value_SYS_memory_missing                = 4,
    eoerror_value_SYS_mutex_timeout                 = 5,
    eoerror_value_SYS_wrongparam                    = 6,
    eoerror_value_SYS_wrongusage                    = 7,
    eoerror_value_SYS_runtimeerror                  = 8,
    eoerror_value_SYS_runninghappily                = 9,
    eoerror_value_SYS_runninginfatalerrorstate      = 10,
    eoerror_value_SYS_ctrloop_execoverflowRX        = 11,
    eoerror_value_SYS_ctrloop_execoverflowDO        = 12,
    eoerror_value_SYS_ctrloop_execoverflowTX        = 13,
    eoerror_value_SYS_udptxfailure                  = 14,
    eoerror_value_SYS_ropparsingerror               = 15,
    eoerror_value_SYS_halerror                      = 16,
    eoerror_value_SYS_osalerror                     = 17,
    eoerror_value_SYS_ipalerror                     = 18,
    eoerror_value_SYS_dispatcherfifooverflow        = 19,
    eoerror_value_SYS_configurator_udptxfailure     = 20,
    eoerror_value_SYS_runner_udptxfailure           = 21,
    eoerror_value_SYS_runner_transceivererror       = 22,
    eoerror_value_SYS_canservices_cantxfailure      = 23
} eOerror_value_SYS_t;

enum { eoerror_value_SYS_numberof = 24 };


/** @typedef    typedef enum eOerror_value_HW_t
    @brief      it contains the values of the eoerror_category_HardWare category. In case you need a new value: add it
                in consecutive mode without holes and increment eoerror_value_HW_numberof. In case you need to remove a value, dont
                remove the number but simply rename it: that will help detecting old boards sending pairs {category, value} not supported yet.  
 **/
typedef enum
{
    eoerror_value_HW_first            = 0,
    eoerror_value_HW_second           = 1
} eOerror_value_HW_t;

enum { eoerror_value_HW_numberof = 2 };



/** @typedef    typedef enum eOerror_value_MC_t
    @brief      it contains the values of the eoerror_category_MotionControl category. In case you need a new value: add it
                in consecutive mode without holes and increment eoerror_value_MC_numberof. In case you need to remove a value, dont
                remove the number but simply rename it: that will help detecting old boards sending pairs {category, value} not supported yet.  
 **/
typedef enum
{
    eoerror_value_MC_first            = 0,
    eoerror_value_MC_second           = 1
} eOerror_value_MC_t;

enum { eoerror_value_MC_numberof = 2 };


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------


extern const eOerror_code_t eoerror_code_dummy;     // = EOERROR_CODE_DUMMY
extern const eOerror_value_t eoerror_value_dummy;   // = EOERROR_VALUE_DUMMY


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern eOerror_code_t eoerror_errorcode_get(eOerror_category_t cat, eOerror_value_t val)
    @brief      it retrieves the error code given a category and a value.
    @param      cat             the category
    @param      val             the value. Use relevant values for the enum of the specific category: eOerror_value_SYS_t, 
                                eOerror_value_HW_t, eOerror_value_MC_t, et cetera.
    @return     the valid error code or eoerror_code_dummy in case of wrong parameters.
 **/
extern eOerror_code_t eoerror_code_get(eOerror_category_t cat, eOerror_value_t val); 


/** @fn         extern eOerror_category_t eoerror_code2category(eOerror_code_t code)
    @brief      it retrieves the category given an error code.
    @param      code            the error code obtained from a message or from function eoerror_code_get(). 

    @return     the valid category or eoerror_category_dummy in case of unsupported code.
 **/
extern eOerror_category_t eoerror_code2category(eOerror_code_t code); 


/** @fn         extern eOerror_value_t eoerror_code2value(eOerror_code_t code)
    @brief      it retrieves the value given an error code.
    @param      code            the error code obtained from a message or from function eoerror_code_get(). 

    @return     the valid value or eoerror_value_dummy in case of unsupported code.
 **/
extern eOerror_value_t eoerror_code2value(eOerror_code_t code); 


/** @fn         extern const char* eoerror_code2string(eOerror_code_t code)
    @brief      it returns a string which describes the error code in argument. 
                if the argument maps to an existing code, then the associated 
                string is returned. Otherwise, it is returned a string with prefix "eoerror_INTERNALERROR__"
                which describes why a valid string could not be found.                
    @param      code            the error code obtained from a message or from function eoerror_code_get()
    @return     a zero-terminated string associated to the error code or an error string with prefix "eoerror_INTERNALERROR__".
 **/
extern const char* eoerror_code2string(eOerror_code_t code);



/** @}            
    end of group eo_EoError  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



