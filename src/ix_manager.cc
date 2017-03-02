//
// File:        ix_indexmanager.cc
// Description: IX_IndexHandle handles indexes
// Author:      Yifei Huang (yifei@stanford.edu)
//

#include <unistd.h>
#include <sys/types.h>
#include "ix_internal.h"
#include "pf.h"
#include <climits>
#include <string>
#include <sstream>
#include <cstdio>
#include "comparators.h"


IX_Manager::IX_Manager(PF_Manager &pfm) :pfm(pfm)
{

}

IX_Manager::~IX_Manager()
{

}

bool IX_Manager::IsValidIndex(AttrType attrType, int attrLength){//////////////////////////////////////////////////////////////////////////////////////////
  if(attrType == INT && attrLength == 4)
    return true;
  else if(attrType == FLOAT && attrLength == 4)
    return true;
  else if(attrType == STRING && attrLength > 0 && attrLength <= MAXSTRINGLEN)
    return true;
  else
    return false;
}

RC IX_Manager::GetIndexFileName(const char *fileName, int indexNo, std::string &indexname){///////////////////////////////////////////////////////////////////
  //RC rc = 0;
  std::stringstream convert;
  convert << indexNo;
  std::string idx_num = convert.str();
  indexname = std::string(fileName);
  indexname.append(".");
  indexname.append(idx_num);
  if(indexname.size() > PATH_MAX || idx_num.size() > 10) // sizes of file and index number
    return (IX_BADINDEXNAME);                            // cannot exceed certain sizes
  return (0);
}

/*
 * Creates a new index given the filename, the index number, attribute type and length.
 */
RC IX_Manager::CreateIndex(const char *fileName, int indexNo,
                   int AttrCount,AttrInfo   *attributes)// TODO: not modify input on the upper level
{
  if(fileName == NULL || indexNo < 0) // Check that the file name and index number are valid
    return (IX_BADFILENAME);
  RC rc = 0;

  for(int i =0;i<AttrCount;i++){	// check that if all the attribute length and type are valid
  	if(! IsValidIndex(attrType, attrLength)) 
    return (IX_BADINDEXSPEC);
  }
  

  // Create index file:
  std::string indexname;
  if((rc = GetIndexFileName(fileName, indexNo, indexname)))
    return (rc);
  if((rc = pfm.CreateFile(indexname.c_str()))){
    return (rc);
  }


  // Initial the header and root node
  PF_PageHandle ph_header;
  PF_PageHandle ph_root;
  if((rc = pfm.OpenFile(indexname.c_str(), fh)))
    return (rc);
 PageNum headerpage;
  PageNum rootpage;
  if((rc = fh.AllocatePage(ph_header)) || (rc = ph_header.GetPageNum(headerpage))
    || (rc = fh.AllocatePage(ph_root)) || (rc = ph_root.GetPageNum(rootpage))){
    return (rc);
  }
  struct IX_IndexHeader *header;
  struct Node_Entry *entries;
  if((rc = ph_header.GetData((char *&) header)) || (rc = ph_root.GetData((char *&) entries))){
    goto cleanup_and_exit; // if this fails, then destroy the file
  }

  // setup header page
   
    int attrCount;
    int entryOffset;

    PageNum rootpage;
  header->attrInfo = attributes;
  header->attrCount = AttrCount;
  header->maxKey= MAX_KEY;

  for(int i=0; i < header->maxKey; i++){
    entries[i].isValid = UNOCCUPIED;
    entries[i].page = NO_MORE_PAGES;
    if(i == (header->maxKeys_N -1))
      entries[i].nextSlot = NO_MORE_SLOTS;
    else
      entries[i].nextSlot = i+1;
  }
 // Mark both pages as dirty, and close the file
  cleanup_and_exit:
  RC rc2;
  if((rc2 = fh.MarkDirty(headerpage)) || (rc2 = fh.UnpinPage(headerpage)) ||
    (rc2 = fh.MarkDirty(rootpage)) || (rc2 = fh.UnpinPage(rootpage)) || (rc2 = pfm.CloseFile(fh)))
    return (rc2);

  return (rc);
}

/*
 * This function destroys a valid index given the file name and index number.
 */
RC IX_Manager::DestroyIndex(const char *fileName, int indexNo)
{

}

/*
 * This function, given a valid fileName and index Number, opens up the
 * index associated with it, and returns it via the indexHandle variable
 */
RC IX_Manager::OpenIndex(const char *fileName, int indexNo,
                 IX_IndexHandle &indexHandle)
{
  
}

/*
 * Given a valid index handle, closes the file associated with it
 */
RC IX_Manager::CloseIndex(IX_IndexHandle &indexHandle)
{
 
}
