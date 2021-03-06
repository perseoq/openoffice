/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_unotools.hxx"

#include <unotools/componentfactory.hxx>
#include <comphelper/processfactory.hxx>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

#ifndef _COM_SUN_STAR_REGISTRY_XREGISTRYKEY_HDL_
#include <com/sun/star/registry/XRegistryKey.hpp>
#endif

#include <cppuhelper/factory.hxx>

#include <uno/environment.h>
#include <uno/mapping.hxx>

#include <rtl/ustring.hxx>
#include <osl/module.h>




using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::registry;
using namespace ::rtl;


namespace utl
{

Reference< XInterface > getComponentInstance(
			const OUString & rLibraryName,
			const OUString & rImplementationName
			)
{
	Reference< XInterface > xI;
	Reference< XMultiServiceFactory > xMSF = ::comphelper::getProcessServiceFactory();
	if ( xMSF.is() )
		xI = xMSF->createInstance( rImplementationName );
	if( !xI.is() )
	{
		Reference< XSingleServiceFactory > xSSF =
			loadLibComponentFactory( rLibraryName, rImplementationName,
			Reference< XMultiServiceFactory >(), Reference< XRegistryKey >() );
		xI = xSSF->createInstance();
	}
	return xI;
}


Reference< XSingleServiceFactory > loadLibComponentFactory(
			const OUString & rLibName,
			const OUString & rImplName,
			const Reference< XMultiServiceFactory > & xSF,
			const Reference< XRegistryKey > & xKey
			)
{
	Reference< XSingleServiceFactory > xRet;

	oslModule lib = osl_loadModule( rLibName.pData, SAL_LOADMODULE_LAZY | SAL_LOADMODULE_GLOBAL );
	if (lib)
	{
		void * pSym;

		// ========================= LATEST VERSION =========================
		OUString aGetEnvName( RTL_CONSTASCII_USTRINGPARAM(COMPONENT_GETENV) );
		if (pSym = osl_getSymbol( lib, aGetEnvName.pData ))
		{
			uno_Environment * pCurrentEnv = 0;
			uno_Environment * pEnv = 0;
			const sal_Char * pEnvTypeName = 0;
			(*((component_getImplementationEnvironmentFunc)pSym))( &pEnvTypeName, &pEnv );

			sal_Bool bNeedsMapping =
				(pEnv || 0 != rtl_str_compare( pEnvTypeName, CPPU_CURRENT_LANGUAGE_BINDING_NAME ));

			OUString aEnvTypeName( OUString::createFromAscii( pEnvTypeName ) );

			if (bNeedsMapping)
			{
				if (! pEnv)
					uno_getEnvironment( &pEnv, aEnvTypeName.pData, 0 );
				if (pEnv)
				{
					OUString aCppEnvTypeName( RTL_CONSTASCII_USTRINGPARAM(CPPU_CURRENT_LANGUAGE_BINDING_NAME) );
					uno_getEnvironment( &pCurrentEnv, aCppEnvTypeName.pData, 0 );
					if (pCurrentEnv)
						bNeedsMapping = (pEnv != pCurrentEnv);
				}
			}

			OUString aGetFactoryName( RTL_CONSTASCII_USTRINGPARAM(COMPONENT_GETFACTORY) );
			if (pSym = osl_getSymbol( lib, aGetFactoryName.pData ))
			{
				OString aImplName( OUStringToOString( rImplName, RTL_TEXTENCODING_ASCII_US ) );

				if (bNeedsMapping)
				{
					if (pEnv && pCurrentEnv)
					{
						Mapping aCurrent2Env( pCurrentEnv, pEnv );
						Mapping aEnv2Current( pEnv, pCurrentEnv );

						if (aCurrent2Env.is() && aEnv2Current.is())
						{
							void * pSMgr = aCurrent2Env.mapInterface(
								xSF.get(), ::getCppuType( (const Reference< XMultiServiceFactory > *)0 ) );
							void * pKey = aCurrent2Env.mapInterface(
								xKey.get(), ::getCppuType( (const Reference< XRegistryKey > *)0 ) );

							void * pSSF = (*((component_getFactoryFunc)pSym))(
								aImplName.getStr(), pSMgr, pKey );

							if (pKey)
								(*pEnv->pExtEnv->releaseInterface)( pEnv->pExtEnv, pKey );
							if (pSMgr)
								(*pEnv->pExtEnv->releaseInterface)( pEnv->pExtEnv, pSMgr );

							if (pSSF)
							{
								aEnv2Current.mapInterface(
									reinterpret_cast< void ** >( &xRet ),
									pSSF, ::getCppuType( (const Reference< XSingleServiceFactory > *)0 ) );
								(*pEnv->pExtEnv->releaseInterface)( pEnv->pExtEnv, pSSF );
							}
						}
					}
				}
				else
				{
					XSingleServiceFactory * pRet = (XSingleServiceFactory *)
						(*((component_getFactoryFunc)pSym))(
							aImplName.getStr(), xSF.get(), xKey.get() );
					if (pRet)
					{
						xRet = pRet;
						pRet->release();
					}
				}
			}

			if (pEnv)
				(*pEnv->release)( pEnv );
			if (pCurrentEnv)
				(*pCurrentEnv->release)( pCurrentEnv );
		}


		if (! xRet.is())
			osl_unloadModule( lib );
	}

	return xRet;
}

}	// namespace utl
