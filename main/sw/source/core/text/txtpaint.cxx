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
#include "precompiled_sw.hxx"



#include "txtpaint.hxx"
#include "swrect.hxx"
#include "rootfrm.hxx"

/*************************************************************************
 *                      SwSaveClip::Reset()
 *************************************************************************/

void SwSaveClip::Reset()
{
	// Der alte Zustand wird wieder hergestellt.
	if( pOut && bChg )
	{
		if ( pOut->GetConnectMetaFile() )
			pOut->Pop();
		else
		{
			if( bOn )
				pOut->SetClipRegion( aClip );
			else
				pOut->SetClipRegion();
		}
		bChg = sal_False;
	}
}

/*************************************************************************
 *						SwSaveClip::_ChgClip()
 *************************************************************************/

void SwSaveClip::_ChgClip( const SwRect &rRect, const SwTxtFrm* pFrm,
                           sal_Bool bEnlargeRect )
{
    SwRect aOldRect( rRect );
    const sal_Bool bVertical = pFrm && pFrm->IsVertical();

    if ( pFrm && pFrm->IsRightToLeft() )
        pFrm->SwitchLTRtoRTL( (SwRect&)rRect );

    if ( bVertical )
        pFrm->SwitchHorizontalToVertical( (SwRect&)rRect );

    if ( !pOut || (!rRect.HasArea() && !pOut->IsClipRegion()) )
    {
        (SwRect&)rRect = aOldRect;
        return;
    }

	if ( !bChg )
	{
		if ( pOut->GetConnectMetaFile() )
			pOut->Push();
		else if ( bOn )
            aClip = pOut->GetClipRegion();
	}

	if ( !rRect.HasArea() )
		pOut->SetClipRegion();
	else
	{
        Rectangle aRect( rRect.SVRect() );

        // Having underscores in our line, we enlarged the repaint area
        // (see frmform.cxx) because for some fonts it could be too small.
        // Consequently, we have to enlarge the clipping rectangle as well.
        if ( bEnlargeRect && ! bVertical )
            aRect.Bottom() += 40;

		// Wenn das ClipRect identisch ist, passiert nix.
        if( pOut->IsClipRegion() ) // kein && wg Mac
        {
            if ( aRect == pOut->GetClipRegion().GetBoundRect() )
            {
                (SwRect&)rRect = aOldRect;
                return;
            }
        }

		if( SwRootFrm::HasSameRect( rRect ) )
			pOut->SetClipRegion();
		else
		{
			const Region aClipRegion( aRect );
			pOut->SetClipRegion( aClipRegion );
#if OSL_DEBUG_LEVEL > 1
            Rectangle aDbgRect = pOut->GetClipRegion().GetBoundRect();
#endif
		}
#if OSL_DEBUG_LEVEL > 1
#ifdef DBG_UTIL
		static sal_Bool bDbg = sal_False;
		if( bDbg )
		{
			DbgBackColor aDbg( pOut, bDbg, COL_RED );
			pOut->DrawRect( aRect );
		}
#endif
#endif
	}
	bChg = sal_True;

    (SwRect&)rRect = aOldRect;
}


