#include <stdexcept>

#include "Directories.h"
#include "HImage.h"
#include "Structure.h"
#include "TileDef.h"
#include "Animation_Cache.h"
#include "Animation_Data.h"
#include "Debug_Control.h"
#include "Tile_Surface.h"
#include "Tile_Cache.h"
#include "FileMan.h"
#include "MemMan.h"
#include "Vector.h"

#include <fstream>
bool fexists(const char *filename)
{
  std::ifstream ifile(filename);
  return ifile;
}


struct TILE_CACHE_STRUCT
{
	char zRootName[30];
	STRUCTURE_FILE_REF* pStructureFileRef;
};


static const UINT32 guiMaxTileCacheSize = 50;
static UINT32 guiCurTileCacheSize       = 0;
static INT32  giDefaultStructIndex      = -1;


TILE_CACHE_ELEMENT*                   gpTileCache;
static SGP::Vector<TILE_CACHE_STRUCT> gpTileCacheStructInfo;


#define FILENAME_SIZE 255

void InitTileCache(void)
{
	gpTileCache         = MALLOCN(TILE_CACHE_ELEMENT, guiMaxTileCacheSize);
	guiCurTileCacheSize = 0;

	// Zero entries
	for (UINT32 i = 0; i < guiMaxTileCacheSize; ++i)
	{
		gpTileCache[i].pImagery        = 0;
		gpTileCache[i].struct_file_ref = 0;
	}

	// Look for JSD files in the tile cache directory and load any we find
	const char* const data_path = GetBinDataPath();
	char              jsd_file_pattern[512];
    // FindFiles is broken on Android - for whatever reason
    // we have to get our list of files manually.
    // These are all files in my tilecache folder in ja2 v1.13
    // probably more than required, but better than forgetting one
    // length is 109
    char *filelist[] = {"c_dead1.jsd",
                        "c_dead1_nb.jsd",
                        "cp_dead1.jsd",
                        "cp_dead1_nb.jsd",
                        "ct_dead1.jsd",
                        "ct_dead1_nb.jsd",
                        "ct_dead.jsd",
                        "ct_dead_nb.jsd",
                        "cw_dead1.jsd",
                        "cw_dead1_nb.jsd",
                        "f_d_bck.jsd",
                        "f_d_bck_nb.jsd",
                        "f_d_dhd.jsd",
                        "f_d_dhd_nb.jsd",
                        "f_dead.jsd",
                        "f_dead_nb.jsd",
                        "f_d_fallf.jsd",
                        "f_d_fallf_nb.jsd",
                        "f_d_fall.jsd",
                        "f_d_fall_nb.jsd",
                        "f_d_fwd.jsd",
                        "f_d_fwd_nb.jsd",
                        "f_d_jfk.jsd",
                        "f_d_jfk_nb.jsd",
                        "f_d_prn.jsd",
                        "f_d_prn_nb.jsd",
                        "f_d_wtr.jsd",
                        "f_d_wtr_nb.jsd",
                        "ft_dead1.jsd",
                        "ft_dead1_nb.jsd",
                        "ft_dead2.jsd",
                        "ft_dead2_nb.jsd",
                        "h_dead1.jsd",
                        "h_dead1_nb.jsd",
                        "h_dead2.jsd",
                        "h_dead2_nb.jsd",
                        "hm_wrek.jsd",
                        "ic_wrek.jsd",
                        "i_dead1.jsd",
                        "i_dead1_nb.jsd",
                        "j_dead.jsd",
                        "k_dead1.jsd",
                        "k_dead1_nb.jsd",
                        "k_dead2.jsd",
                        "k_dead2_nb.jsd",
                        "l_dead1.jsd",
                        "l_dead1_nb.jsd",
                        "m_bck.jsd",
                        "m_bck_nb.jsd",
                        "m_d_bck.jsd",
                        "m_d_bck_nb.jsd",
                        "m_d_dhd.jsd",
                        "m_d_dhd_nb.jsd",
                        "m_dead1.jsd",
                        "m_dead1_nb.jsd",
                        "m_dead2.jsd",
                        "m_dead2_nb.jsd",
                        "m_d_fallf.jsd",
                        "m_d_fallf_nb.jsd",
                        "m_d_fall.jsd",
                        "m_d_fall_nb.jsd",
                        "m_d_fwd.jsd",
                        "m_d_fwd_nb.jsd",
                        "m_d_jfk.jsd",
                        "m_d_jfk_nb.jsd",
                        "m_d_prn.jsd",
                        "m_d_prn_nb.jsd",
                        "m_d_wtr.jsd",
                        "m_d_wtr_nb.jsd",
                        "m_jfk.jsd",
                        "m_jfk_nb.jsd",
                        "mn_dead1.jsd",
                        "mn_dead1_nb.jsd",
                        "mn_dead2.jsd",
                        "mn_dead2_nb.jsd",
                        "p_decomp2.jsd",
                        "q_dead.jsd",
                        "q_dead_nb.jsd",
                        "qn_dead.jsd",
                        "s_burnt.jsd",
                        "s_d_bck.jsd",
                        "s_d_bck_nb.jsd",
                        "s_d_dhd.jsd",
                        "s_d_dhd_nb.jsd",
                        "s_dead1.jsd",
                        "s_dead1_nb.jsd",
                        "s_dead2.jsd",
                        "s_dead2_nb.jsd",
                        "s_d_fallf.jsd",
                        "s_d_fallf_nb.jsd",
                        "s_d_fall.jsd",
                        "s_d_fall_nb.jsd",
                        "s_d_fwd.jsd",
                        "s_d_fwd_nb.jsd",
                        "s_d_jfk.jsd",
                        "s_d_jfk_nb.jsd",
                        "s_d_prn.jsd",
                        "s_d_prn_nb.jsd",
                        "s_d_wtr.jsd",
                        "s_d_wtr_nb.jsd",
                        "s_expld.jsd",
                        "tk2_wrek.jsd",
                        "tk2_wrek_nb.jsd",
                        "tk_wrek.jsd",
                        "tk_wrek_nb.jsd",
                        "w_dead1.jsd",
                        "w_dead1_nb.jsd",
                        "w_dead2.jsd",
                        "w_dead2_nb.jsd"};

	snprintf(jsd_file_pattern, lengthof(jsd_file_pattern), "%s/" BASEDATADIR "/" TILECACHEDIR "/*.jsd", data_path);

	int manual_filelist_counter = 0;
	// Loop through and set filenames
	SGP::FindFiles find(jsd_file_pattern);
	for (;;)
	{
//		char const* const find_filename = find.Next();
//		if (find_filename == NULL) break;
        if (manual_filelist_counter > 108) break;
        char const* const find_filename = filelist[manual_filelist_counter];
		manual_filelist_counter++;


		char filename[FILENAME_SIZE] = "";
		sprintf(filename, "%s/" BASEDATADIR "/" TILECACHEDIR "/%s", data_path, find_filename);

        if (!fexists(filename)) continue; // avoid errors

		TILE_CACHE_STRUCT tc = {0};
		GetRootName(tc.zRootName, lengthof(tc.zRootName), filename);
		tc.pStructureFileRef = LoadStructureFile(filename);

		if (strcasecmp(tc.zRootName, "l_dead1") == 0)
		{
			giDefaultStructIndex = gpTileCacheStructInfo.Size();
		}

		gpTileCacheStructInfo.PushBack(tc);
	}
}


void DeleteTileCache( )
{
	UINT32 cnt;

	// Allocate entries
	if ( gpTileCache != NULL )
	{
		// Loop through and delete any entries
		for ( cnt = 0; cnt < guiMaxTileCacheSize; cnt++ )
		{
			if ( gpTileCache[ cnt ].pImagery != NULL )
			{
				DeleteTileSurface( gpTileCache[ cnt ].pImagery );
			}
		}
		MemFree( gpTileCache );
	}

	gpTileCacheStructInfo.Reserve(0);

	guiCurTileCacheSize = 0;
}


INT32 GetCachedTile(const char* const filename)
{
	INT32 idx = -1;

	// Check to see if surface exists already
	for (UINT32 cnt = 0; cnt < guiCurTileCacheSize; ++cnt)
	{
		TILE_CACHE_ELEMENT* const i = &gpTileCache[cnt];
		if (i->pImagery == NULL)
		{
			if (idx == -1) idx = cnt;
			continue;
		}

		if (strcasecmp(i->zName, filename) != 0) continue;

		// Found surface, return
		++i->sHits;
		return (INT32)cnt;
	}

	if (idx == -1)
	{
		if (guiCurTileCacheSize < guiMaxTileCacheSize)
		{
			idx = guiCurTileCacheSize++;
		}
		else
		{
			// cache out least used file
			idx = 0;
			INT16 sMostHits = gpTileCache[idx].sHits;
			for (UINT32 cnt = 1; cnt < guiCurTileCacheSize; ++cnt)
			{
				const TILE_CACHE_ELEMENT* const i = &gpTileCache[cnt];
				if (i->sHits < sMostHits)
				{
					sMostHits = i->sHits;
					idx       = cnt;
				}
			}

			// Bump off lowest index
			TILE_CACHE_ELEMENT* const del = &gpTileCache[idx];
			DeleteTileSurface(del->pImagery);
			del->sHits           = 0;
			del->pImagery        = 0;
			del->struct_file_ref = 0;
		}
	}

	TILE_CACHE_ELEMENT* const tce = &gpTileCache[idx];

	tce->pImagery = LoadTileSurface(filename);

	strcpy(tce->zName, filename);
	tce->sHits = 1;

	char root_name[30];
	GetRootName(root_name, lengthof(root_name), filename);
	STRUCTURE_FILE_REF* const sfr = GetCachedTileStructureRefFromFilename(root_name);
	tce->struct_file_ref = sfr;
	if (sfr) AddZStripInfoToVObject(tce->pImagery->vo, sfr, TRUE, 0);

	const AuxObjectData* const aux = tce->pImagery->pAuxData;
	tce->ubNumFrames = (aux != NULL ? aux->ubNumberOfFrames : 1);

	return idx;
}


void RemoveCachedTile(INT32 const cached_tile)
{
	if ((UINT32)cached_tile < guiCurTileCacheSize)
	{
		TILE_CACHE_ELEMENT& e = gpTileCache[cached_tile];
		if (e.pImagery)
		{
			if (--e.sHits != 0) return;

			DeleteTileSurface(e.pImagery);
			e.pImagery        = 0;
			e.struct_file_ref = 0;
			return;
		}
	}
	throw std::logic_error("Trying to remove invalid cached tile");
}


static STRUCTURE_FILE_REF* GetCachedTileStructureRef(INT32 const idx)
{
	return idx != -1 ? gpTileCache[idx].struct_file_ref : 0;
}


STRUCTURE_FILE_REF* GetCachedTileStructureRefFromFilename(char const* const filename)
{
	size_t const n = gpTileCacheStructInfo.Size();
	for (size_t i = 0; i != n; ++i)
	{
		TILE_CACHE_STRUCT& t = gpTileCacheStructInfo[i];
		if (strcasecmp(t.zRootName, filename) == 0) return t.pStructureFileRef;
	}
	return 0;
}


void CheckForAndAddTileCacheStructInfo(LEVELNODE* const pNode, INT16 const sGridNo, UINT16 const usIndex, UINT16 const usSubIndex)
{
	STRUCTURE_FILE_REF* const sfr = GetCachedTileStructureRef(usIndex);
	if (!sfr)  return;

	if (AddStructureToWorld(sGridNo, 0, &sfr->pDBStructureRef[usSubIndex], pNode)) return;

	if (giDefaultStructIndex == -1) return;

	STRUCTURE_FILE_REF* const def_sfr = gpTileCacheStructInfo[giDefaultStructIndex].pStructureFileRef;
	if (!def_sfr) return;

	AddStructureToWorld(sGridNo, 0, &def_sfr->pDBStructureRef[usSubIndex], pNode);
}


void CheckForAndDeleteTileCacheStructInfo( LEVELNODE *pNode, UINT16 usIndex )
{
	STRUCTURE_FILE_REF *pStructureFileRef;

	if ( usIndex >= TILE_CACHE_START_INDEX )
	{
		pStructureFileRef = GetCachedTileStructureRef( ( usIndex - TILE_CACHE_START_INDEX ) );

		if ( pStructureFileRef != NULL)
		{
			DeleteStructureFromWorld( pNode->pStructureData );
		}
	}
}


void GetRootName(char* const pDestStr, size_t const n, char const* const pSrcStr)
{
	// Remove path and extension
	ReplacePath(pDestStr, n, "", pSrcStr, "");
}
