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
#include "precompiled_canvas.hxx"

#include <canvas/debug.hxx>
#include <canvas/canvastools.hxx>

#include <rtl/math.hxx>

#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <basegfx/point/b2dpoint.hxx>
#include <basegfx/range/b2drectangle.hxx>
#include <basegfx/tools/canvastools.hxx>
#include <basegfx/numeric/ftools.hxx>
#include <basegfx/tools/tools.hxx>

#include <limits>

#include <canvas/parametricpolypolygon.hxx>


using namespace ::com::sun::star;

namespace canvas
{
    uno::Sequence<rtl::OUString> ParametricPolyPolygon::getAvailableServiceNames()
    {
        uno::Sequence<rtl::OUString> aRet(3);
        aRet[0] = rtl::OUString::createFromAscii("LinearGradient");
        aRet[1] = rtl::OUString::createFromAscii("EllipticalGradient");
        aRet[2] = rtl::OUString::createFromAscii("RectangularGradient");

        return aRet;
    }

    ParametricPolyPolygon* ParametricPolyPolygon::create( 
        const uno::Reference< rendering::XGraphicDevice >& rDevice,
        const ::rtl::OUString& rServiceName,
        const uno::Sequence< uno::Any >& rArgs )
    {
        uno::Sequence< uno::Sequence< double > > colorSequence(2);
        uno::Sequence< double > colorStops(2);
        double fAspectRatio=1.0;

        // defaults
        uno::Sequence< rendering::RGBColor > rgbColors(1);
        rgbColors[0] = rendering::RGBColor(0,0,0);
        colorSequence[0] = rDevice->getDeviceColorSpace()->convertFromRGB(rgbColors);
        rgbColors[0] = rendering::RGBColor(1,1,1);
        colorSequence[1] = rDevice->getDeviceColorSpace()->convertFromRGB(rgbColors);
        colorStops[0] = 0;
        colorStops[1] = 1;

        // extract args
        for( sal_Int32 i=0; i<rArgs.getLength(); ++i )
        {
            beans::PropertyValue aProp;
            if( (rArgs[i] >>= aProp) )
            {
                if( aProp.Name.equalsAscii("Colors") )
                {
                    aProp.Value >>= colorSequence;
                }
                else if( aProp.Name.equalsAscii("Stops") )
                {
                    aProp.Value >>= colorStops;
                }
                else if( aProp.Name.equalsAscii("AspectRatio") )
                {
                    aProp.Value >>= fAspectRatio;
                }
            }
        }

        if( rServiceName.equalsAscii("LinearGradient") )
        {
            return createLinearHorizontalGradient(rDevice, colorSequence, colorStops);
        }
        else if( rServiceName.equalsAscii("EllipticalGradient") )
        {
            return createEllipticalGradient(rDevice, colorSequence, colorStops, fAspectRatio);
        }
        else if( rServiceName.equalsAscii("RectangularGradient") )
        {
            return createRectangularGradient(rDevice, colorSequence, colorStops, fAspectRatio);
        }
        else if( rServiceName.equalsAscii("VerticalLineHatch") )
        {
            // TODO: NYI
        }
        else if( rServiceName.equalsAscii("OrthogonalLinesHatch") )
        {
            // TODO: NYI
        }
        else if( rServiceName.equalsAscii("ThreeCrossingLinesHatch") )
        {
            // TODO: NYI
        }
        else if( rServiceName.equalsAscii("FourCrossingLinesHatch") )
        {
            // TODO: NYI
        }

        return NULL;
    }

    ParametricPolyPolygon* ParametricPolyPolygon::createLinearHorizontalGradient( 
        const uno::Reference< rendering::XGraphicDevice >& 	rDevice,
        const uno::Sequence< uno::Sequence< double > >&		colors, 
        const uno::Sequence< double >& 						stops )
    {
        // TODO(P2): hold gradient brush statically, and only setup
        // the colors
        return new ParametricPolyPolygon( rDevice, GRADIENT_LINEAR, colors, stops );
    }

    ParametricPolyPolygon* ParametricPolyPolygon::createEllipticalGradient( 
        const uno::Reference< rendering::XGraphicDevice >& 	rDevice,
        const uno::Sequence< uno::Sequence< double > >&		colors, 
        const uno::Sequence< double >& 						stops,
        double                                              fAspectRatio )
    {
        // TODO(P2): hold gradient polygon statically, and only setup
        // the colors
        return new ParametricPolyPolygon( 
            rDevice,
            ::basegfx::tools::createPolygonFromCircle( 
                ::basegfx::B2DPoint(0,0), 1 ),
            GRADIENT_ELLIPTICAL,
            colors, stops, fAspectRatio );
    }

    ParametricPolyPolygon* ParametricPolyPolygon::createRectangularGradient( const uno::Reference< rendering::XGraphicDevice >& rDevice,
                                                                             const uno::Sequence< uno::Sequence< double > >&	colors, 
                                                                             const uno::Sequence< double >& 					stops,
                                                                             double                                             fAspectRatio )
    {
        // TODO(P2): hold gradient polygon statically, and only setup
        // the colors
        return new ParametricPolyPolygon( 
            rDevice,
            ::basegfx::tools::createPolygonFromRect( 
                ::basegfx::B2DRectangle( -1, -1, 1, 1 ) ),
            GRADIENT_RECTANGULAR,
            colors, stops, fAspectRatio );
    }

    void SAL_CALL ParametricPolyPolygon::disposing()
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        mxDevice.clear();
    }

    uno::Reference< rendering::XPolyPolygon2D > SAL_CALL ParametricPolyPolygon::getOutline( double /*t*/ ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        // TODO(F1): outline NYI
        return uno::Reference< rendering::XPolyPolygon2D >();
    }

    uno::Sequence< double > SAL_CALL ParametricPolyPolygon::getColor( double /*t*/ ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        // TODO(F1): color NYI
        return uno::Sequence< double >();
    }

    uno::Sequence< double > SAL_CALL ParametricPolyPolygon::getPointColor( const geometry::RealPoint2D& /*point*/ ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        // TODO(F1): point color NYI
        return uno::Sequence< double >();
    }

    uno::Reference< rendering::XColorSpace > SAL_CALL ParametricPolyPolygon::getColorSpace() throw (uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        return mxDevice.is() ? mxDevice->getDeviceColorSpace() : uno::Reference< rendering::XColorSpace >();
    }

#define IMPLEMENTATION_NAME "Canvas::ParametricPolyPolygon"
#define SERVICE_NAME "com.sun.star.rendering.ParametricPolyPolygon"

    ::rtl::OUString SAL_CALL ParametricPolyPolygon::getImplementationName(  ) throw (uno::RuntimeException)
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( IMPLEMENTATION_NAME ) );
    }

    sal_Bool SAL_CALL ParametricPolyPolygon::supportsService( const ::rtl::OUString& ServiceName ) throw (uno::RuntimeException)
    {
        return ServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( SERVICE_NAME ) );
    }

    uno::Sequence< ::rtl::OUString > SAL_CALL ParametricPolyPolygon::getSupportedServiceNames(  ) throw (uno::RuntimeException)
    {
        uno::Sequence< ::rtl::OUString > aRet(1);
        aRet[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( SERVICE_NAME ) );

        return aRet;
    }

    ParametricPolyPolygon::~ParametricPolyPolygon()
    {
    }

    ParametricPolyPolygon::ParametricPolyPolygon( const uno::Reference< rendering::XGraphicDevice >& 	rDevice,
                                                  const ::basegfx::B2DPolygon&							rGradientPoly,
                                                  GradientType                                          eType,
                                                  const uno::Sequence< uno::Sequence< double > >&		rColors, 
                                                  const uno::Sequence< double >& 						rStops ) :
        ParametricPolyPolygon_Base( m_aMutex ),
        mxDevice( rDevice ),
        maValues( rGradientPoly,
                  rColors,
                  rStops,
                  1.0,
                  eType )
    {
    }

    ParametricPolyPolygon::ParametricPolyPolygon( const uno::Reference< rendering::XGraphicDevice >& 	rDevice,
                                                  const ::basegfx::B2DPolygon&							rGradientPoly,
                                                  GradientType                                          eType,
                                                  const uno::Sequence< uno::Sequence< double > >&		rColors, 
                                                  const uno::Sequence< double >& 						rStops,
                                                  double												nAspectRatio ) :
        ParametricPolyPolygon_Base( m_aMutex ),        
        mxDevice( rDevice ),
        maValues( rGradientPoly,
                  rColors,
                  rStops,
                  nAspectRatio,
                  eType )
    {
    }

    ParametricPolyPolygon::ParametricPolyPolygon( const uno::Reference< rendering::XGraphicDevice >& 	rDevice,
                                                  GradientType	 										eType,
                                                  const uno::Sequence< uno::Sequence< double > >&		rColors, 
                                                  const uno::Sequence< double >& 						rStops ) :
        ParametricPolyPolygon_Base( m_aMutex ),        
        mxDevice( rDevice ),
        maValues( ::basegfx::B2DPolygon(),
                  rColors,
                  rStops, 
                  1.0,
                  eType )
    {
    }

    ParametricPolyPolygon::Values ParametricPolyPolygon::getValues() const
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        return maValues;
    }

}
