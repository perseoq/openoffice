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
#include "precompiled_extensions.hxx"
#include "formcontroller.hxx"
#include "pcrcommon.hxx"
#include "formstrings.hxx"
#include "defaultforminspection.hxx"
#ifndef _EXTENSIONS_FORMCTRLR_FORMHELPID_HRC_
#include "propctrlr.hrc"
#endif

/** === begin UNO includes === **/
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/form/XGridColumnFactory.hpp>
#include <com/sun/star/form/XForm.hpp>
#include <com/sun/star/container/XChild.hpp>
/** === end UNO includes === **/
#include <cppuhelper/typeprovider.hxx>
#include <toolkit/helper/vclunohelper.hxx>

//------------------------------------------------------------------------
extern "C" void SAL_CALL createRegistryInfo_FormController()
{
	::pcr::OAutoRegistration< ::pcr::FormController > aFormControllerRegistration;
	::pcr::OAutoRegistration< ::pcr::DialogController > aDialogControllerRegistration;
}

//........................................................................
namespace pcr
{
//........................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::TypeClass_INTERFACE;
    using ::com::sun::star::uno::TypeClass_STRING;
    using ::com::sun::star::uno::XComponentContext;
    using ::com::sun::star::inspection::XObjectInspectorModel;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::uno::XInterface;
    using ::com::sun::star::beans::XPropertySetInfo;
    using ::com::sun::star::beans::XPropertySet;
    using ::com::sun::star::beans::Property;
    using ::com::sun::star::uno::Any;
    using ::com::sun::star::lang::IllegalArgumentException;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::uno::Type;
    using ::com::sun::star::util::VetoException;
    using ::com::sun::star::beans::PropertyVetoException;
    using ::com::sun::star::uno::UNO_QUERY;
    using ::com::sun::star::form::XGridColumnFactory;
    using ::com::sun::star::form::XForm;
    using ::com::sun::star::container::XChild;
    using ::com::sun::star::frame::XFrame;
    using ::com::sun::star::awt::XWindow;
    /** === end UNO using === **/

    namespace PropertyAttribute = ::com::sun::star::beans::PropertyAttribute;

	//====================================================================
	//= FormController
	//====================================================================
	//--------------------------------------------------------------------
    FormController::FormController( const Reference< XComponentContext >& _rxContext, ServiceDescriptor _aServiceDescriptor,
            bool _bUseFormFormComponentHandlers )
        :OPropertyBrowserController( _rxContext )
        ,FormController_PropertyBase1( m_aBHelper )
        ,m_aServiceDescriptor( _aServiceDescriptor )
    {
        osl_incrementInterlockedCount( &m_refCount );
        {
            Reference< XObjectInspectorModel > xModel(
                *(new DefaultFormComponentInspectorModel( _rxContext, _bUseFormFormComponentHandlers )),
                UNO_QUERY_THROW
            );
            setInspectorModel( xModel );
        }
        osl_decrementInterlockedCount( &m_refCount );
    }

	//------------------------------------------------------------------------
    FormController::~FormController()
    {
    }

	//------------------------------------------------------------------------
    IMPLEMENT_FORWARD_XINTERFACE2( FormController, OPropertyBrowserController, FormController_PropertyBase1 )

	//------------------------------------------------------------------------
	Sequence< Type > SAL_CALL FormController::getTypes(  ) throw(RuntimeException)
	{
		::cppu::OTypeCollection aTypes(
			::getCppuType( static_cast< Reference< XPropertySet >* >(NULL) ),
			::getCppuType( static_cast< Reference< XMultiPropertySet >* >(NULL) ),
			::getCppuType( static_cast< Reference< XFastPropertySet >* >(NULL) ),
			OPropertyBrowserController::getTypes());
		return aTypes.getTypes();
	}

	//------------------------------------------------------------------------
	IMPLEMENT_GET_IMPLEMENTATION_ID( FormController )

	//------------------------------------------------------------------------
	::rtl::OUString SAL_CALL FormController::getImplementationName(  ) throw(RuntimeException)
	{
        return m_aServiceDescriptor.GetImplementationName();
	}

	//------------------------------------------------------------------------
	Sequence< ::rtl::OUString > SAL_CALL FormController::getSupportedServiceNames(  ) throw(RuntimeException)
	{
		Sequence< ::rtl::OUString > aSupported( m_aServiceDescriptor.GetSupportedServiceNames() );
        aSupported.realloc( aSupported.getLength() + 1 );
        aSupported[ aSupported.getLength() - 1 ] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.inspection.ObjectInspector" ) );
        return aSupported;
	}

	//------------------------------------------------------------------------
	::rtl::OUString FormController::getImplementationName_static(  ) throw(RuntimeException)
	{
		return ::rtl::OUString::createFromAscii("org.openoffice.comp.extensions.FormController");
	}

	//------------------------------------------------------------------------
	Sequence< ::rtl::OUString > FormController::getSupportedServiceNames_static(  ) throw(RuntimeException)
	{
		Sequence< ::rtl::OUString > aSupported(1);
		aSupported[0] = ::rtl::OUString::createFromAscii( "com.sun.star.form.PropertyBrowserController" );
		return aSupported;
	}

	//------------------------------------------------------------------------
	Reference< XInterface > SAL_CALL FormController::Create(const Reference< XComponentContext >& _rxContext )
	{
        ServiceDescriptor aService;
        aService.GetImplementationName = &FormController::getImplementationName_static;
        aService.GetSupportedServiceNames = &FormController::getSupportedServiceNames_static;
        return *(new FormController( _rxContext, aService, true ) );
	}

	//------------------------------------------------------------------------
	Reference< XPropertySetInfo > SAL_CALL FormController::getPropertySetInfo(  ) throw(RuntimeException)
	{
		return ::cppu::OPropertySetHelper::createPropertySetInfo(getInfoHelper());
	}

	//------------------------------------------------------------------------
	::cppu::IPropertyArrayHelper& SAL_CALL FormController::getInfoHelper()
	{
		return *getArrayHelper();
	}

	//------------------------------------------------------------------------
	::cppu::IPropertyArrayHelper* FormController::createArrayHelper( ) const
	{
	    Sequence< Property > aProps( 2 );
        aProps[0] = Property(
            PROPERTY_CURRENTPAGE,
            OWN_PROPERTY_ID_CURRENTPAGE,
            ::getCppuType( static_cast< ::rtl::OUString* >( NULL ) ),
			PropertyAttribute::TRANSIENT
        );
        aProps[1] = Property(
            PROPERTY_INTROSPECTEDOBJECT,
            OWN_PROPERTY_ID_INTROSPECTEDOBJECT,
            XPropertySet::static_type(),
			PropertyAttribute::TRANSIENT | PropertyAttribute::CONSTRAINED
        );
	    return new ::cppu::OPropertyArrayHelper( aProps );
	}

    //------------------------------------------------------------------------
    sal_Bool SAL_CALL FormController::convertFastPropertyValue( Any & rConvertedValue, Any & rOldValue, sal_Int32 nHandle, const Any& rValue ) throw (IllegalArgumentException)
    {
        switch ( nHandle )
        {
        case OWN_PROPERTY_ID_INTROSPECTEDOBJECT:
            if ( rValue.getValueTypeClass() != TypeClass_INTERFACE )
                throw IllegalArgumentException();
            break;
        case OWN_PROPERTY_ID_CURRENTPAGE:
            if ( rValue.getValueTypeClass() != TypeClass_STRING )
                throw IllegalArgumentException();
            break;
        }

        getFastPropertyValue( rOldValue, nHandle );
        rConvertedValue = rValue;
        return sal_True;
    }

    //------------------------------------------------------------------------
	void SAL_CALL FormController::setFastPropertyValue_NoBroadcast(sal_Int32 _nHandle, const Any& _rValue) throw (Exception)
	{
		switch ( _nHandle )
		{
		case OWN_PROPERTY_ID_INTROSPECTEDOBJECT:
        {
            Reference< XObjectInspectorModel > xModel( getInspectorModel() );
            if ( xModel.is() )
            {
                try
                {
                    m_xCurrentInspectee.set( _rValue, UNO_QUERY );
                    Sequence< Reference< XInterface > > aObjects;
                    if ( m_xCurrentInspectee.is() )
                    {
                        aObjects.realloc( 1 );
                        aObjects[0] = m_xCurrentInspectee;
                    }

                    Reference< XObjectInspector > xInspector( *this, UNO_QUERY_THROW );
                    xInspector->inspect( aObjects );
                }
                catch( const VetoException& e )
                {
                    throw PropertyVetoException( e.Message, e.Context );
                }
            }
        }
		break;
        case OWN_PROPERTY_ID_CURRENTPAGE:
			restoreViewData( _rValue );
			break;
		}
	}

    //------------------------------------------------------------------------
	void SAL_CALL FormController::getFastPropertyValue( ::com::sun::star::uno::Any& rValue, sal_Int32 nHandle ) const
    {
		switch ( nHandle )
		{
		case OWN_PROPERTY_ID_INTROSPECTEDOBJECT:
            rValue <<= m_xCurrentInspectee;
		    break;

        case OWN_PROPERTY_ID_CURRENTPAGE:
			rValue = const_cast< FormController* >( this )->getViewData();
			break;
		}
    }

	//====================================================================
	//= DialogController
	//====================================================================
    //------------------------------------------------------------------------
    ::rtl::OUString DialogController::getImplementationName_static(  ) throw(RuntimeException)
    {
		return ::rtl::OUString::createFromAscii("org.openoffice.comp.extensions.DialogController");
    }

    //------------------------------------------------------------------------
	Sequence< ::rtl::OUString > DialogController::getSupportedServiceNames_static(  ) throw(RuntimeException)
    {
		Sequence< ::rtl::OUString > aSupported(1);
		aSupported[0] = ::rtl::OUString::createFromAscii( "com.sun.star.awt.PropertyBrowserController" );
		return aSupported;
    }

    //------------------------------------------------------------------------
	Reference< XInterface > SAL_CALL DialogController::Create(const Reference< XComponentContext >& _rxContext)
    {
        ServiceDescriptor aService;
        aService.GetImplementationName = &DialogController::getImplementationName_static;
        aService.GetSupportedServiceNames = &DialogController::getSupportedServiceNames_static;
        return *(new FormController( _rxContext, aService, false ) );
    }

//........................................................................
} // namespace pcr
//........................................................................
