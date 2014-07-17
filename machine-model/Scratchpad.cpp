#include "./Scratchpad.h"
const unsigned MEM_size[]  = {64, 128, 256, 512, 1024, 2049, 4098, 8196, 16392, 32784, 65568, 131136, 262272};
const float MEM_rd_power[] = {1.779210, 1.779210, 2.653500, 2.653500, 3.569050, 4.695780, 5.883620, 7.587260, 9.458480, 8.363850, 13.472600, 12.640600, 18.336900};
const float MEM_wr_power[] = {1.733467, 1.733467, 2.531965, 2.531965, 3.138079, 3.783919, 4.450720, 5.007659, 5.370660, 4.590109, 7.371770, 5.849070, 6.549049};
const float MEM_lk_power[] = {0.013156, 0.026312, 0.052599, 0.105198, 0.210474, 0.420818, 0.841640, 1.682850, 3.365650, 6.729040, 13.459700, 26.916200, 53.832100};
const float MEM_area[]     = {1616.140000, 2929.000000, 4228.290000, 7935.990000, 15090.200000, 28129.300000, 49709.900000, 94523.900000, 174459.000000, 352194.000000, 684305.000000, 1319220.000000, 2554980.000000};


Scratchpad::Scratchpad(unsigned p_ports_per_part)
{
  numOfPortsPerPartition = p_ports_per_part;
}
Scratchpad::~Scratchpad()
{}

void Scratchpad::setScratchpad(string baseName, unsigned size)
{
  assert(!partitionExist(baseName));
  
  unsigned new_id = baseToPartitionID.size();
  baseToPartitionID[baseName] = new_id;
  occupiedBWPerPartition.push_back(0);
  sizePerPartition.push_back(size);
  //set
  unsigned mem_size = next_power_of_two(size);
  if (mem_size < 64)
    mem_size = 64;
  unsigned mem_index = (unsigned) log2(mem_size) - 6;
  readPowerPerPartition.push_back(MEM_rd_power[mem_index]);
  writePowerPerPartition.push_back(MEM_wr_power[mem_index]);
  leakPowerPerPartition.push_back(MEM_lk_power[mem_index]);
  areaPerPartition.push_back(MEM_area[mem_index]);
}

void Scratchpad::step()
{
  for (auto it = occupiedBWPerPartition.begin(); it != occupiedBWPerPartition.end(); ++it)
    *it = 0;
}
bool Scratchpad::partitionExist(string baseName)
{
  auto partition_it = baseToPartitionID.find(baseName);
  if (partition_it != baseToPartitionID.end())
    return 1;
  else
    return 0;
}
bool Scratchpad::addressRequest(string baseName)
{
  if (canServicePartition(baseName))
  {
    unsigned partition_id = findPartitionID(baseName);
    assert(occupiedBWPerPartition.at(partition_id) < numOfPortsPerPartition);
    occupiedBWPerPartition.at(partition_id)++;
    return true;
  }
  else
    return false;
}
bool Scratchpad::canService()
{
  for(auto base_it = baseToPartitionID.begin(); base_it != baseToPartitionID.end(); ++base_it)
  {
    string base_name = base_it->first;
    if (canServicePartition(base_name))
      return 1;
  }
  return 0;
}
bool Scratchpad::canServicePartition(string baseName)
{
  unsigned partition_id = findPartitionID(baseName);
  if (occupiedBWPerPartition.at(partition_id) < numOfPortsPerPartition)
    return 1;
  else
    return 0;
}

unsigned Scratchpad::findPartitionID(string baseName)
{
  auto partition_it = baseToPartitionID.find(baseName);
  if (partition_it != baseToPartitionID.end())
    return partition_it->second;
  else
  {
    cerr << "Unknown Partition Name:" << baseName << endl;
    exit(0);
  }
}

void Scratchpad::partitionNames(std::vector<string> &names)
{
  for(auto base_it = baseToPartitionID.begin(); base_it != baseToPartitionID.end(); ++base_it)
  {
    string base_name = base_it->first;
    names.push_back(base_name);
  }
}

float Scratchpad::readPower (std::string baseName)
{
  unsigned partition_id = findPartitionID(baseName);
  return readPowerPerPartition.at(partition_id);
}

float Scratchpad::writePower (std::string baseName)
{
  unsigned partition_id = findPartitionID(baseName);
  return writePowerPerPartition.at(partition_id);
}

float Scratchpad::leakPower (std::string baseName)
{
  unsigned partition_id = findPartitionID(baseName);
  return leakPowerPerPartition.at(partition_id);
}

float Scratchpad::area (std::string baseName)
{
  unsigned partition_id = findPartitionID(baseName);
  return areaPerPartition.at(partition_id);
}
