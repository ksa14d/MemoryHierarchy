/*************************************************************
 *                                                           *
 *                                                           *
 *                                                           *
 *  Name: Karthik Shankar Achalkar(ksa14d)                   *
 *                                                           *
 *                                                           *
 *  Class: CDA5155                                           *
 *                                                           *
 *  Assignment: Implementing memory hierarchy Simulator      *
 *                                                           *
 *                                                           *
 *  Compile: "make"                                          *
 *                                                           *
 * 							     *
 *							     *
 *************************************************************/
#include<iostream>
#include<fstream>
#include<climits>
#include<stdio.h>
#include<string>
#include<math.h>
using namespace std;
bool IsVAEnabled = false;
bool IsTLBEnabled = false;
bool IsData = false;
class Stats;
class Instruction;
class LineDetails;
class TLB;
class InstructionCache;
class PageTable;
class DataCache;
class Stats
{
public:
    double itlb_hits;
    double itlb_misses;
    double itlb_hit_ratio;

    double dtlb_hits;
    double dtlb_misses;
    double dtlb_hit_ratio;

    double pt_hits;
    double pt_faults;
    double pt_hit_ratio;

    double ic_hits;
    double ic_misses;
    double ic_hit_ratio;

    double dc_hits;
    double dc_misses;
    double dc_hit_ratio;

    double total_reads;
    double total_writes;
    double read_ratio;

    double total_inst_ref;
    double total_data_ref;
    double inst_ratio;

    double memref;
    double diskref;
    Stats()
    {
        itlb_hits = itlb_misses =  dtlb_hits = dtlb_misses  = pt_hits =  pt_faults = ic_hits = ic_misses = dc_hits = dc_misses =  total_reads = total_writes  = total_inst_ref = total_data_ref =  memref = diskref = 0.0;
        itlb_hit_ratio = dtlb_hit_ratio = pt_hit_ratio = ic_hit_ratio = dc_hit_ratio = read_ratio = inst_ratio = 0.0;
    }

    void ComputeRatio()
    {
        itlb_hit_ratio = itlb_hits / (itlb_hits + itlb_misses);
        dtlb_hit_ratio = dtlb_hits / (dtlb_hits + dtlb_misses);

        pt_hit_ratio = pt_hits /(pt_hits +  pt_faults);

        ic_hit_ratio = ic_hits / (ic_hits + ic_misses);
        dc_hit_ratio = dc_hits / (dc_hits + dc_misses) ;
        read_ratio = total_reads / (total_reads + total_writes);
        inst_ratio = total_inst_ref / (total_inst_ref + total_data_ref);
    }
    void PrintInfo()
    {
        printf("\nSimulation statistics\n\n");

        printf("itlb hits        : %.0lf\n",itlb_hits);
        printf("itlb misses      : %.0lf\n",itlb_misses);
        if((itlb_misses + itlb_hits)  != 0)
        {
            printf("itlb hit ratio   : %lf\n\n",itlb_hit_ratio);
        }
        else
        {
            printf("itlb hit ratio   : N/A\n\n");
        }

        printf("dtlb hits        : %.0lf\n",dtlb_hits);
        printf("dtlb misses      : %.0lf\n",dtlb_misses);
        if((dtlb_misses+dtlb_hits) != 0)
        {
            printf("dtlb hit ratio   : %lf\n\n",dtlb_hit_ratio);
        }
        else
        {
            printf("dtlb hit ratio   : N/A\n\n");
        }

        printf("pt hits          : %.0lf\n",pt_hits);
        printf("pt faults        : %.0lf\n",pt_faults);
        if((pt_faults+pt_hits) != 0)
        {
            printf("pt hit ratio     : %lf\n\n",pt_hit_ratio);
        }
        else
        {
            printf("pt hit ratio     : N/A\n\n");
        }

        printf("ic hits          : %.0lf\n",ic_hits);
        printf("ic misses        : %.0lf\n",ic_misses);
        if((ic_misses+ic_hits) != 0)
        {
            printf("ic hit ratio     : %lf\n\n",ic_hit_ratio);
        }
        else
        {
            printf("ic hit ratio     : N/A\n\n");
        }

        printf("dc hits          : %.0lf\n",dc_hits);
        printf("dc misses        : %.0lf\n",dc_misses);
        if((dc_misses+dc_hits) != 0)
        {
            printf("dc hit ratio     : %lf\n\n",dc_hit_ratio);
        }
        else
        {
            printf("dc hit ratio     : N/A\n\n");
        }


        printf("Total reads      : %.0lf\n",total_reads);
        printf("Total writes     : %.0lf\n",total_writes);
        if((total_writes+total_reads) != 0)
        {
            printf("Ratio of reads   : %lf\n\n",read_ratio);
        }
        else
        {
            printf("Ratio of reads   : N/A\n\n");
        }

        printf("Total inst refs  : %.0lf\n",total_inst_ref);
        printf("Total data refs  : %.0lf\n",total_data_ref);
        if((total_data_ref+total_inst_ref) != 0)
        {
            printf("Ratio of insts   : %lf\n\n",inst_ratio);
        }
        else
        {
            printf("Ratio of insts   : N/A\n\n");
        }
        printf("main memory refs : %.0lf\n",memref);
        printf("disk refs        : %.0lf\n",diskref);

    }
};
Stats *stats = new Stats();
class BitMask
{
public:
    BitMask() {}
    int indexBits ;
    int offsetBits;
    int tagBits ;
    unsigned indexMask ;
    unsigned offsetMask;
    unsigned tagMask ;
    void Initialize()
    {
        indexMask  = (2 << (indexBits-1)) - 1;
        offsetMask = (2 << (offsetBits - 1)) - 1;
        tagMask =    (2 << (tagBits-1)) - 1;
    }
};


class LineDetails
{
public:
    LineDetails()
    {
        IsValid = false;
        IsResident = false;
        frequency = 0;
    }
    void parse(unsigned Address, BitMask m)
    {
        // compute offset index and tag
        offset = m.offsetMask & Address ;
        Address >>= m.offsetBits;
        index = m.indexMask & Address;
        Address >>= m.indexBits;
        tag = m.tagMask & Address;
        IsValid = true;
        IsDirty = false;
    }
    unsigned tag ;            //used for TLB and Cache
    unsigned offset;          //used for TLB and Cache
    unsigned index;           // used for TLB and Cache
    unsigned PhysicalAddress; // used for TLB
    unsigned VirtualAddress;// invalidate TLB
    unsigned DiskAddress; // used for TLB page fault
    unsigned ppageno; // used to invalidate cache line
    int frequency;  // LRU
    bool IsValid; //
    bool IsDirty;  // write back
    bool IsResident; // Disk or mainmem
};



class TLB
{
public:
    TLB() {}
    LineDetails*** sets; // divide this into sets and while placing a item use LRU
    int n_sets;
    int set_size;
    int page_size;
    bool IsHit;
    BitMask mask;
    int* LRU ;
    // need to have a page Table Reference to get physical address details on TLB misses
    void Initialize()
    {
        LRU = new int[n_sets];
        sets = new LineDetails**[n_sets];
        for(int i = 0; i< n_sets ; i++)
        {
            LRU[i] = 1;
            sets[i] = new LineDetails*[set_size];
        }
        for(int i = 0; i < n_sets ; i++ )
        {
            for(int j = 0; j< set_size; j++)
            {
                sets[i][j] = new LineDetails();
            }
        }
        //printf("Set size : %d\n",set_size);
        mask.indexBits = (int)log2(n_sets);
        if(mask.indexBits == 0)mask.indexBits = 1;
        mask.offsetBits = (int)log2(page_size);
        if(mask.offsetBits == 0)mask.offsetBits = 1;
        mask.tagBits = 32 - (mask.indexBits + mask.offsetBits);
        if(mask.tagBits == 0)mask.tagBits = 1;
        mask.Initialize();
    }

    unsigned GetPhysicalAddress(unsigned VAddress,int* vpageno,int* ppageno,int* page_offset,int* tlb_tag,int* tlb_index,string* tlb_ref,string* pt_ref);   // GetInstruction
    void InvalidateEntry(unsigned VAddress)
    {
        //   printf("*************invalidating TLB********************\n");
        LineDetails *dtls = new LineDetails();
        dtls->parse(VAddress,mask);

//	    printf("%x %x %x\n",dtls->tag,dtls->index,dtls->offset);
        for(int i = 0; i < set_size ; i++ )
        {
            if(sets[dtls->index][i]->IsValid && sets[dtls->index][i]->tag == dtls->tag) // data is valid compare the tag
            {
                sets[dtls->index][i]->frequency = 0;
                sets[dtls->index][i]->IsValid = false;
                break;
            }
        }
        delete dtls;
    }

};

class InstructionCache
{
public:
    InstructionCache() {}
    LineDetails*** sets; // divide this into sets and while placing a item use LRU
    int n_sets;
    int set_size;
    int line_size;
    BitMask mask;
    int * LRU;
    void Initialize()
    {
        LRU = new int[n_sets];
        sets = new LineDetails**[n_sets];
        for(int i = 0; i< n_sets ; i++)
        {
            LRU[i] = 1;
            sets[i] = new LineDetails*[set_size];
        }
        for(int i = 0; i < n_sets ; i++ )
        {
            for(int j = 0; j< set_size; j++)
            {
                sets[i][j] = new LineDetails();
            }
        }
        //printf("Set size : %d\n",set_size);
        mask.indexBits = (int)log2(n_sets);
        if(mask.indexBits == 0)mask.indexBits = 1;
        mask.offsetBits = (int)log2(line_size);
        if(mask.offsetBits == 0)mask.offsetBits = 1;
        mask.tagBits = 32 - (mask.indexBits + mask.offsetBits);
        if(mask.tagBits == 0)mask.tagBits = 1;
        mask.Initialize();
    }

    void InvalidateAllEntry(unsigned ppageno)
    {
        for(int j = 0; j < n_sets ; j++ )
        {
            for(int i = 0; i < set_size ; i++ )
            {
                if(sets[j][i]->IsValid && sets[j][i]->ppageno == ppageno) // data is valid compare the tag
                {

                    // printf("*************invalidating IC********************\n");
                    sets[j][i]->IsValid = false;
                }
            }
        }
    }

    bool GetInstruction(unsigned PAddress,unsigned ppageno,int* cache_tag,int* cache_index,string* cache_ref)   // GetInstruction
    {
        // break down  the physical address and update the cache
        //see if the line is available at a index
        LineDetails *dtls = new LineDetails();
        dtls->parse(PAddress,mask);
        *cache_index = dtls->index;
        *cache_tag = dtls->tag;
        bool IsHit = false;
        for(int i = 0; i < set_size ; i++ )
        {
            if(sets[dtls->index][i]->IsValid && sets[dtls->index][i]->tag == dtls->tag) // data is valid compare the tag
            {
                sets[dtls->index][i]->frequency = LRU[dtls->index]++; // use the data
                *cache_ref = "hit ";
                IsHit = true;
                break;
            }
        }
        if(!IsHit) // miss pull data into cache do a main mem access  Note : Data will be in main mem
        {
            stats->memref++;
            *cache_ref = "miss";
            int small = sets[dtls->index][0]->frequency;
            int idx = 0;
            for(int i =0; i < set_size ; i++ )  // find the least frequency
            {
                if(sets[dtls->index][i]->frequency < small) // data is valid compare the tag
                {
                    small = sets[dtls->index][i]->frequency;
                    idx = i;
                }
            }
            sets[dtls->index][idx]->parse(PAddress,mask);
            sets[dtls->index][idx]->ppageno = ppageno; // used during invalidating
            sets[dtls->index][idx]->frequency = LRU[dtls->index]++; // use the data
        }
        //printf("tag : %x , Index : %u, offset : %u, Result : %u\n",dtls->tag,dtls->index,dtls->offset,IsHit);
        delete dtls;
        return IsHit; // Hit ?
    }
};



class DataCache
{
public:

    DataCache() {}
    int n_sets;
    int set_size; // associativity
    int line_size;
    LineDetails*** sets; // divide this into sets and while placing a item use LRU
    bool IsWriteThrough ;
    BitMask mask;
    int* LRU;

    void Initialize()
    {
        LRU = new int[n_sets];
        sets = new LineDetails**[n_sets];
        for(int i = 0; i< n_sets ; i++)
        {
            LRU[i] = 1;
            sets[i] = new LineDetails*[set_size];
        }
        for(int i = 0; i < n_sets ; i++ )
        {
            for(int j = 0; j< set_size; j++)
            {
                sets[i][j] = new LineDetails();
            }
        }
        mask.indexBits = (int)log2(n_sets);
        if(mask.indexBits == 0)mask.indexBits = 1;
        mask.offsetBits = (int)log2(line_size);
        if(mask.offsetBits == 0)mask.offsetBits = 1;
        mask.tagBits = 32 - (mask.indexBits + mask.offsetBits);
        if(mask.tagBits == 0)mask.tagBits = 1;
        mask.Initialize();
    }

    void InvalidateAllEntry(unsigned PAddress,int vpageno);

    bool ReadData(unsigned PAddress,unsigned ppageno, int vpageno,int* cache_tag,int* cache_index,string* cache_ref);

    bool WriteBack(unsigned PAddress,unsigned ppageno,int vpageno,int* cache_tag,int* cache_index,string* cache_ref) ;

    bool WriteThrough(unsigned PAddress,int vpageno,int* cache_tag,int* cache_index,string* cache_ref); // write to main mem is compulsory where as write to cache is optional
};

InstructionCache *ic = new InstructionCache();
DataCache *dc = new DataCache();
TLB *itlb = new TLB();
TLB *dtlb = new TLB();
class PageTable
{
public:
    PageTable() {}
    int n_vpages;
    unsigned n_ppages;
    int page_size;
    unsigned pagesInUse;
// need to have a TLB reference here to ivalidate the memory upon fault LRU
    LineDetails* PageMap[8192];
    int  LRU;
    BitMask mask;
    void Initialize()
    {

        LRU = 1;
        for(int i = 0; i< n_vpages ; i++)
        {
            PageMap[i] = new LineDetails();
        }
        mask.indexBits = (int)log2(n_vpages);
        if(mask.indexBits == 0)mask.indexBits = 1;
        mask.offsetBits = (int)log2(page_size);
        if(mask.offsetBits == 0)mask.offsetBits = 1;
        mask.tagBits = 32 - (mask.indexBits + mask.offsetBits);
        if(mask.tagBits == 0)mask.tagBits = 1;
        mask.Initialize();
    }

    unsigned GetPhysicalPageNum(unsigned VAddress,int* vpageno, int* ppageno,int* page_offset,string* pt_ref) // vaddr to index in to page table
    {
        stats->memref++;
        LineDetails *dtls = new LineDetails();
        dtls->parse(VAddress,mask);
        bool IsHit = false;

        if(PageMap[dtls->index]->IsResident)
        {
            // hit
            IsHit = true;
            *pt_ref = "hit ";
            PageMap[dtls->index]->frequency = LRU++; // use the data
            return PageMap[dtls->index]->PhysicalAddress;
        }
        if(!IsHit)   // page fault
        {
            *pt_ref = "miss";
            stats->diskref++;// pulling data from disk
            if(pagesInUse < n_ppages)
            {
                PageMap[dtls->index]->PhysicalAddress = pagesInUse;   // physical page num
                pagesInUse++;
                PageMap[dtls->index]->frequency = LRU++; // use the data
                PageMap[dtls->index]->IsResident = true;
            }
            else
            {
                int small = INT_MAX ;
                int idx = 0;
                for(int i =0; i < n_vpages ; i++ )  // find the least frequency
                {
                    if(PageMap[i]->IsResident && PageMap[i]->frequency < small)
                    {
                        small = PageMap[i]->frequency;
                        idx = i;
                    }
                }
                // invalidate cache entry also if it is dirty write back
                //unsigned PhysicalAddress =  (PageMap[idx]->PhysicalAddress << mask.offsetBits) | ( mask.offsetMask & PageMap[idx]->VirtualAddress );
                // printf("%x \n",PageMap[idx]->PhysicalAddress);
                dc->InvalidateAllEntry(PageMap[idx]->PhysicalAddress,idx); // happen on page faults  invalidate TLB aand cache line passing ppageno
                if(IsTLBEnabled)
                    dtlb->InvalidateEntry(PageMap[idx]->VirtualAddress); // happen on page faults  invalidate TLB aand cache line
                ic->InvalidateAllEntry(PageMap[idx]->PhysicalAddress); // happen on page faults  invalidate TLB aand cache lin passing ppageno
                if(IsTLBEnabled)
                    itlb->InvalidateEntry(PageMap[idx]->VirtualAddress); // happen on page faults
                if(PageMap[idx]->IsDirty) // check if it is dirty because of invalidation
                {
                    stats->diskref++; //writing data to disk
                }
                PageMap[idx]->IsResident = false;
                PageMap[dtls->index]->PhysicalAddress = PageMap[idx]->PhysicalAddress; // physical page num
                PageMap[dtls->index]->frequency = LRU++; // use the data
                PageMap[dtls->index]->IsResident = true;
            }
            PageMap[dtls->index]->VirtualAddress = VAddress;
        }
        unsigned pa = PageMap[dtls->index]->PhysicalAddress;
        *vpageno = dtls->index;
        *ppageno = pa;
        *page_offset = dtls->offset;
        //printf("%x %x %x\n",dtls->index,pa,dtls->offset);
        delete dtls;
        return pa;
    }


};

PageTable *pt = new PageTable();
bool DataCache::ReadData(unsigned PAddress,unsigned ppageno,int vpageno,int* cache_tag,int* cache_index,string* cache_ref)   // Read Data
{
    // break down  the physical address and update the cache
    //see if the line is available at a index
    LineDetails *dtls = new LineDetails();
    dtls->parse(PAddress,mask);
    *cache_index = dtls->index;
    *cache_tag = dtls->tag;
    bool IsHit = false;
    for(int i = 0; i < set_size ; i++ )
    {
        if(sets[dtls->index][i]->IsValid && sets[dtls->index][i]->tag == dtls->tag) // data is valid compare the tag
        {
            sets[dtls->index][i]->frequency = LRU[dtls->index]++; // use the data
            *cache_ref = "hit ";
            IsHit = true;
            break;
        }
    }
    if(!IsHit) // miss pull data into cache and place it using LRU do a main mem access future work
    {
        stats->memref++;
        *cache_ref = "miss";
        int small = sets[dtls->index][0]->frequency;
        int idx = 0;
        for(int i =0; i < set_size ; i++ )  // find the least frequency
        {
            if(sets[dtls->index][i]->frequency < small) // data is valid compare the tag
            {
                small = sets[dtls->index][i]->frequency;
                idx = i;
            }
        }

        if(sets[dtls->index][idx]->IsDirty)// write back check for write back
        {
            // if something in cache it is always in mainmemory : inclusion principle
            stats->memref++;  //on a cache miss
            // make pt dirty for that
            pt->PageMap[vpageno]->IsDirty = true;
        }
        sets[dtls->index][idx]->parse(PAddress,mask);
        sets[dtls->index][idx]->ppageno = ppageno; // used during invalidating
        sets[dtls->index][idx]->frequency = LRU[dtls->index]++; // use the data
    }
    //printf("tag : %x , Index : %u, offset : %u, Result : %u\n",dtls->tag,dtls->index,dtls->offset,IsHit);
    delete dtls;
    return IsHit; // Hit ?
}

// write to cache is compulsory where as write to main memory happens to be at the end
bool DataCache::WriteBack(unsigned PAddress,unsigned ppageno,int vpageno,int* cache_tag,int* cache_index,string* cache_ref)// Write Back With Write allocate
{

    LineDetails *dtls = new LineDetails();
    dtls->parse(PAddress,mask);
    *cache_tag = dtls->tag;
    *cache_index = dtls->index;
    bool IsHit = false;
    for(int i = 0; i < set_size ; i++ )
    {
        if(sets[dtls->index][i]->IsValid && sets[dtls->index][i]->tag == dtls->tag) // data is valid compare the tag
        {
            sets[dtls->index][i]->frequency = LRU[dtls->index]++; // use the data
            sets[dtls->index][i]->IsDirty = true;
            // set this dirty to page table
            IsHit = true;
            break;
        }
    }
    if(!IsHit) // miss pull data into cache do a main mem access future work
    {
        int small = sets[dtls->index][0]->frequency;
        int idx = 0;
        for(int i =0; i < set_size ; i++ )  // find the least frequency
        {
            if(sets[dtls->index][i]->frequency < small) // data is valid compare the tag
            {
                small = sets[dtls->index][i]->frequency;
                idx = i;
            }
        }
        if(sets[dtls->index][idx]->IsDirty)// write back
        {
            // do a TLB access and update the bloc in main mem if not in main mem do a disk access to get it
            stats->memref++;
            pt->PageMap[vpageno]->IsDirty = true;
            // to do : update the ISDirty bit in dTLB and Page table

        }
        sets[dtls->index][idx]->parse(PAddress,mask);
        stats->memref++;
        sets[dtls->index][idx]->ppageno = ppageno; // used during invalidating
        sets[dtls->index][idx]->IsDirty = true;
        sets[dtls->index][idx]->frequency = LRU[dtls->index]++; // use the data
    }
    //printf("tag : %u , Index : %u, offset : %u, Result : %u\n",dtls->tag,dtls->index,dtls->offset,IsHit);
    delete dtls;
    if(IsHit)*cache_ref = "hit ";
    else *cache_ref = "miss";
    return IsHit; // Hit ?
}
bool DataCache::WriteThrough(unsigned PAddress,int vpageno,int* cache_tag,int* cache_index,string* cache_ref) // write to main mem is compulsory where as write to cache is optional , write through - no write allocate
{
    LineDetails *dtls = new LineDetails();
    dtls->parse(PAddress,mask);
    *cache_tag = dtls->tag;
    *cache_index = dtls->index;

    bool IsHit = false;
    for(int i = 0; i < set_size ; i++ )
    {
        if(sets[dtls->index][i]->IsValid && sets[dtls->index][i]->tag == dtls->tag) // data is valid compare the tag
        {
            sets[dtls->index][i]->frequency = LRU[dtls->index]++; // use the data
            //sets[i][dtls->index]->IsDirty = true;
            IsHit = true;   // in the next line do a TLB access and update frequency if data not the its a disk ref
            break;
        }
    }
    //printf("tag : %u , Index : %u, offset : %u, Result : %u\n",dtls->tag,dtls->index,dtls->offset,IsHit);
    // irrespective of a hit or miss mem access is compulsory in write through , no need to pull data in cache
    stats->memref++;
    pt->PageMap[vpageno]->IsDirty = true;
    // to do : update the ISDirty bit in dTLB and Page table
    delete dtls;
    if(IsHit)*cache_ref = "hit ";
    else *cache_ref = "miss";
    return IsHit; // Hit ?
}

void DataCache::InvalidateAllEntry(unsigned ppageno,int vpageno)  //Invalidate all DC lines corresponding to physical page in the pt on fault
{
    // printf("*************invalidating DC********************\n");

    for(int j = 0; j < n_sets ; j++ )
    {
        for(int i = 0; i < set_size ; i++ )
        {
            if(sets[j][i]->IsValid && sets[j][i]->ppageno == ppageno) // data is valid compare the tag
            {
                if(!IsWriteThrough && sets[j][i]->IsDirty)
                {
                    // printf("writing to DC line tag :%x index %x\n",sets[j][i]->tag,sets[j][i]->index);
                    stats->memref++; // write it back when dirty
                }
                if(sets[j][i]->IsDirty)
                {
                    pt->PageMap[vpageno]->IsDirty = true;
                }
                //  printf("invalidating DC line tag :%x index %x\n",sets[j][i]->tag,sets[j][i]->index);
                sets[j][i]->IsValid = false;
            }
        }
    }
}

unsigned TLB::GetPhysicalAddress(unsigned VAddress,int* vpageno,int* ppageno,int* page_offset,int* tlb_tag,int* tlb_index,string* tlb_ref,string* pt_ref)//returns physical address
{
    // break down  the physical address and update the cache
    //see if the line is available at a index
    unsigned PhysicalAddress = 0;
    LineDetails *dtls = new LineDetails();
    dtls->parse(VAddress,mask);
    LineDetails *pagedtls = new LineDetails();
    pagedtls->parse(VAddress,pt->mask);
    *pt_ref = "none";
    *vpageno = pagedtls->index;
    *ppageno = pt->PageMap[pagedtls->index]->PhysicalAddress;
    *page_offset = pagedtls->offset;
    IsHit = false;
    if(IsTLBEnabled)   // bypass TLB
    {
        for(int i = 0; i < set_size ; i++ )
        {
            if(sets[dtls->index][i]->IsValid && sets[dtls->index][i]->tag == dtls->tag) // data is valid compare the tag
            {
                sets[dtls->index][i]->frequency = LRU[dtls->index]++; // use the data
                IsHit = true;
                *tlb_ref = "hit ";
                pt->PageMap[pagedtls->index]->frequency = pt->LRU++; // use the data
                //append block offset to physical address
                PhysicalAddress =  (sets[dtls->index][i]->PhysicalAddress << mask.offsetBits) | ( mask.offsetMask & VAddress );
                break;
            }
        }
    }
    if(!IsHit) // miss pull data into TLB do a page Table access future work
    {
        int small = sets[dtls->index][0]->frequency;
        int idx = 0;
        for(int i =0; i < set_size ; i++ )  // find the least frequency
        {
            if(sets[dtls->index][i]->frequency < small) // data is valid compare the tag
            {
                small = sets[dtls->index][i]->frequency;
                idx = i;
            }
        }
        sets[dtls->index][idx]->parse(VAddress,mask);
        sets[dtls->index][idx]->PhysicalAddress = pt->GetPhysicalPageNum(VAddress,vpageno,ppageno,page_offset,pt_ref); // if fault get it from main mem
        sets[dtls->index][idx]->frequency = LRU[dtls->index]++; // use the data
        PhysicalAddress =  (sets[dtls->index][idx]->PhysicalAddress << mask.offsetBits) | ( mask.offsetMask & VAddress );
        *tlb_ref = "miss";
    }
    *tlb_tag = dtls->tag;
    *tlb_index = dtls->index;

//     printf("tag : %x , Index : %u, offset : %u, Result : %u\n",dtls->tag,dtls->index,dtls->offset,IsHit);
    delete dtls;
    return PhysicalAddress;
}


class Instruction
{
public:
    unsigned physicalAddress;
    unsigned virtualAddress;
    char streamType ;
    char accessType ;
    int ppageno;
    int vpageno;
    int page_offset;
    string reftype; // inst or data
    int tlb_tag;
    int tlb_index;
    string tlb_ref;   //hit or miss
    string pt_ref;  // hit or miss or none
    int cache_tag;
    int cache_index;
    string cache_ref;
    Instruction(string line)
    {
        if(IsVAEnabled)
            sscanf(line.c_str(),"%c:%c:%x",&streamType,&accessType,&virtualAddress);
        else
            sscanf(line.c_str(),"%c:%c:%x",&streamType,&accessType,&physicalAddress);
    }
    void execute()
    {
        if(IsVAEnabled)    // virtual address translation goes here
        {
            if(streamType == 'I')
            {
                IsData = false;
                physicalAddress = itlb->GetPhysicalAddress(virtualAddress,&vpageno,&ppageno,&page_offset,&tlb_tag,&tlb_index,&tlb_ref,&pt_ref);
                reftype = "inst";
                if(IsTLBEnabled)
                {
                    if(tlb_ref.compare("miss")==0) stats->itlb_misses++;
                    else stats->itlb_hits++;
                }
            }
            else
            {
                IsData = true;
                physicalAddress = dtlb->GetPhysicalAddress(virtualAddress,&vpageno,&ppageno,&page_offset,&tlb_tag,&tlb_index,&tlb_ref,&pt_ref);
                reftype = "data";
                if(IsTLBEnabled)
                {
                    if(tlb_ref.compare("miss")==0) stats->dtlb_misses++;
                    else stats->dtlb_hits++;
                }
            }
            if(pt_ref.compare("miss")==0) stats->pt_faults++;
            else if(pt_ref.compare("hit ")==0) stats->pt_hits++;
        }
        LineDetails *dtls = new LineDetails();
        dtls->parse(physicalAddress,pt->mask);
        page_offset = dtls->offset;
        ppageno =  physicalAddress>>pt->mask.offsetBits;
        bool IsHit = false;
        if(streamType == 'I')    //cache access starts here
        {
            IsData = false;
            stats->total_inst_ref++;
            reftype = "inst";
            stats->total_reads++;
            IsHit =  ic->GetInstruction(physicalAddress,ppageno,&cache_tag,&cache_index,&cache_ref);
            if(IsHit)stats->ic_hits++;
            else stats->ic_misses++;
        }
        else
        {
            IsData = true;
            reftype = "data";
            stats->total_data_ref++;
            if(accessType == 'R')
            {
                IsHit =  dc->ReadData(physicalAddress,ppageno,vpageno,&cache_tag,&cache_index,&cache_ref);
                stats->total_reads++;
            }
            else if(accessType == 'W')
            {
                if(dc->IsWriteThrough) IsHit = dc->WriteThrough(physicalAddress,vpageno,&cache_tag,&cache_index,&cache_ref);
                else  IsHit =  dc->WriteBack(physicalAddress,ppageno,vpageno,&cache_tag,&cache_index,&cache_ref);
                stats->total_writes++;
            }
            if(IsHit)stats->dc_hits++;
            else stats->dc_misses++;
        }
        unsigned Address;
        if(IsVAEnabled)Address = virtualAddress;
        else Address = physicalAddress;
        if(IsTLBEnabled && IsVAEnabled)
        {
            printf("%08x %7x %6x %4s %7x %5x %4s %4s %6x %7x %5x %4s\n",Address,vpageno,page_offset,reftype.c_str(),tlb_tag,tlb_index,tlb_ref.c_str(),pt_ref.c_str(),ppageno,cache_tag,cache_index,cache_ref.c_str());
        }
        else if(!IsTLBEnabled && IsVAEnabled)
        {
            printf("%08x %7x %6x %4s %7s %5s %4s %4s %6x %7x %5x %4s\n",Address,vpageno,page_offset,reftype.c_str(),"","","",pt_ref.c_str(),ppageno,cache_tag,cache_index,cache_ref.c_str());

        }
        else if(!IsVAEnabled && !IsTLBEnabled)
        {
            printf("%08x %7s %6x %4s %7s %5s %4s %4s %6x %7x %5x %4s\n",Address,"",page_offset,reftype.c_str(),"","","","",ppageno,cache_tag,cache_index,cache_ref.c_str());
        }
    }
};


int main(int argc, char** argv)
{
    ifstream configFile("trace.config");
    string line;
    Instruction* inst[100];
    int n_inst = 0;
    char s[200];
    int value = 0;
    while( getline( configFile, line ) )
    {
        if(line.find("Instruction TLB")!= string::npos)
        {
            getline( configFile, line );
            sscanf(line.c_str(),"%[^:]:%d",s,&value);
            itlb->n_sets = value;
            getline( configFile, line );
            sscanf(line.c_str(),"%[^:]:%d",s,&value);
            itlb->set_size = value;
        }
        else if(line.find("Data TLB")!= string::npos)
        {
            getline( configFile, line );
            sscanf(line.c_str(),"%[^:]:%d",s,&value);
            dtlb->n_sets = value;
            getline( configFile, line );
            sscanf(line.c_str(),"%[^:]:%d",s,&value);
            dtlb->set_size = value;
        }
        else if(line.find("Page Table")!= string::npos)
        {
            getline( configFile, line );
            sscanf(line.c_str(),"%[^:]:%d",s,&value);
            pt->n_vpages = value;
            getline( configFile, line );
            sscanf(line.c_str(),"%[^:]:%d",s,&value);
            pt->n_ppages = value;
            getline( configFile, line );
            sscanf(line.c_str(),"%[^:]:%d",s,&value);
            pt->page_size = value;
            itlb->page_size = value;
            dtlb->page_size = value;
            pt->Initialize();
            itlb->Initialize();
            dtlb->Initialize();
        }
        else if(line.find("Instruction Cache")!= string::npos)
        {
            getline( configFile, line );
            sscanf(line.c_str(),"%[^:]:%d",s,&value);
            ic->n_sets = value;
            getline( configFile, line );
            sscanf(line.c_str(),"%[^:]:%d",s,&value);
            ic->set_size = value;
            getline( configFile, line );
            sscanf(line.c_str(),"%[^:]:%d",s,&value);
            ic->line_size = value;
            ic->Initialize();
        }
        else if(line.find("Data Cache")!= string::npos)
        {
            getline( configFile, line );
            sscanf(line.c_str(),"%[^:]:%d",s,&value);
            dc->n_sets = value;
            getline( configFile, line );
            sscanf(line.c_str(),"%[^:]:%d",s,&value);
            dc->set_size = value;
            getline( configFile, line );
            sscanf(line.c_str(),"%[^:]:%d",s,&value);
            dc->line_size = value;
            getline( configFile, line );
            if(line.find(": y")!= string::npos )
            {
                dc->IsWriteThrough = true ;
            }
            dc->Initialize();
        }
        else if(line.find("Virtual addr")!= string::npos)
        {
            if(line.find(": y")!= string::npos )
            {
                IsVAEnabled = true;
            }
            else
            {
                IsVAEnabled = false;
            }
        }
        else if(line.find("TLBs")!=string::npos)
        {
            if(line.find(": y")!= string::npos )
            {
                IsTLBEnabled = true;
            }
            else
            {
                IsTLBEnabled = false;
            }
        }
    }

    printf("Instruction TLB contains %d sets.\n",itlb->n_sets);
    printf("Each set contains %d entries.\n",itlb->set_size);
    printf("Number of bits used for the index is %d.\n\n",(int)log2(itlb->n_sets));

    printf("Data TLB contains %d sets.\n",dtlb->n_sets);
    printf("Each set contains %d entries.\n",dtlb->set_size);
    printf("Number of bits used for the index is %d.\n\n",(int)log2(dtlb->n_sets));

    printf("Number of virtual pages is %d.\n",pt->n_vpages);
    printf("Number of physical pages is %d.\n",pt->n_ppages);
    printf("Each page contains %d bytes.\n",pt->page_size);
    printf("Number of bits used for the page table index is %d.\n",(int)log2(pt->n_vpages));
    printf("Number of bits used for the page offset is %d.\n\n",(int)log2(pt->page_size));

    printf("I-cache contains %d sets.\n",ic->n_sets);
    printf("Each set contains %d entries.\n",ic->set_size);
    printf("Each line is %d bytes.\n",ic->line_size);
    printf("Number of bits used for the index is %d.\n",ic->mask.indexBits);
    printf("Number of bits used for the offset is %d.\n\n",ic->mask.offsetBits);


    printf("D-cache contains %d sets.\n",dc->n_sets);
    printf("Each set contains %d entries.\n",dc->set_size);
    printf("Each line is %d bytes.\n",dc->line_size);
    if(dc->IsWriteThrough)
        printf("The cache uses a no write-allocate and write-through policy.\n");
    else
        printf("The cache uses a write-allocate and write-back policy.\n");
    printf("Number of bits used for the index is %d.\n",dc->mask.indexBits);
    printf("Number of bits used for the offset is %d.\n\n",dc->mask.offsetBits);

    if(IsVAEnabled)
        printf("The addresses read in are virtual addresses.\n");
    else
        printf("The addresses read in are physical addresses.\n");

    if(!IsTLBEnabled)
        printf("TLBs are disabled in this configuration.\n");
    if(IsVAEnabled)
        printf("\nVirtual  Virtual Page   Ref  TLB     TLB   TLB  PT   Phys   Cache   Cache Cache\n");
    else
        printf("\nPhysical Virtual Page   Ref  TLB     TLB   TLB  PT   Phys   Cache   Cache Cache\n");
    printf("Address  Page #  Offset Type Tag     Index Ref  Ref  Page # Tag     Index Ref \n");
    printf("-------- ------- ------ ---- ------- ----- ---- ---- ------ ------- ----- -----\n");
    while( getline( cin, line ) )
    {
        inst[n_inst++] = new Instruction(line);
        inst[n_inst - 1]->execute();
    }
    stats->ComputeRatio();
    stats->PrintInfo();

    /*

     cout<<"Number of sets: 2"<<endl;
    */
    return 0;

}
