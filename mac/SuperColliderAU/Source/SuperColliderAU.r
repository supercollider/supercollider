/*
	SuperColliderAU Copyright (c) 2006 Gerard Roma.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */


#include <AudioUnit/AudioUnit.r>

#include "SuperColliderAUVersion.h"

// Note that resource IDs must be spaced 2 apart for the 'STR ' name and description
#define kAudioUnitResID_SuperColliderAU				1000

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SuperColliderAU~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define RES_ID			kAudioUnitResID_SuperColliderAU
#define COMP_TYPE		kAudioUnitType_MusicEffect
//#define COMP_TYPE		kAudioUnitType_Effect
#define COMP_SUBTYPE	SuperColliderAU_COMP_SUBTYPE
#define COMP_MANUF		SuperColliderAU_COMP_MANF	

#define VERSION			kSuperColliderAUVersion
#define NAME			"SuperColliderAU: SuperColliderAU"
#define DESCRIPTION		"SuperColliderAU AU"
#define ENTRY_POINT		"SuperColliderAUEntry"





#include "AUResources.r"