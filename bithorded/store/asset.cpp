/*
    Copyright 2012 <copyright holder> <email>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#include "asset.hpp"

using namespace std;

Asset::Asset(const boost::filesystem3::path& filePath, const boost::filesystem3::path& metaPath) :
  _file(filePath),
  _metaStore(metaPath, _file.blocks(BLOCKSIZE)),
  _hasher(_metaStore)
{
}

size_t Asset::can_read(uint64_t offset, size_t size)
{
	size_t res = 0;
	uint currentBlock = offset / BLOCKSIZE;
	uint endBlockNum = (offset + size) / BLOCKSIZE;
	size_t currentBlockSize = BLOCKSIZE - (offset % BLOCKSIZE);

	while (currentBlock <= endBlockNum && _hasher.isBlockSet(currentBlock)) {
		res += currentBlockSize;

		currentBlock += 1;
		if (currentBlock == endBlockNum)
			currentBlockSize = (offset+size) % BLOCKSIZE;
		else
			currentBlockSize = BLOCKSIZE;
	}

	return res;
}

bool Asset::getIds(BitHordeIds& ids)
{
	BOOST_ASSERT( ids.size() == 0 );
	TigerNode& root = _hasher.getRoot();

	if (root.state == TigerNode::State::SET) {
		auto tigerId = ids.Add();
		tigerId->set_type(bithorde::TREE_TIGER);
		tigerId->set_id(root.digest, TigerNode::DigestSize);
		return true;
	} else {
		return false;
	}
}

bool Asset::hasRootHash()
{
	TigerNode& root = _hasher.getRoot();
	return (root.state == TigerNode::State::SET);
}

uint64_t roundUp(uint64_t val, uint64_t block) {
	size_t overflow = val % block;
	if (overflow)
		return val - overflow;
	else
		return val;
}

uint64_t roundDown(uint64_t val, uint64_t block) {
	return (val / block) * block;
}

void Asset::notifyValidRange(uint64_t offset, uint64_t size)
{
	uint64_t filesize = Asset::size();

	offset = roundUp(offset, BLOCKSIZE);
	uint64_t end = offset + size;
	if (end != filesize)
		end = roundDown(end, BLOCKSIZE);

	updateHash(offset, end);
}

const byte* Asset::read(uint64_t offset, size_t& size, byte* buf)
{
	// TODO: Implement
	return NULL;
}

uint64_t Asset::size() {
	return _file.size();
}

boost::filesystem3::path Asset::storageFile()
{
	return _file.path();
}

boost::filesystem3::path Asset::metaFile()
{
	return _metaStore.path();
}

void Asset::updateHash(uint64_t offset, uint64_t end)
{
	byte BUF[BLOCKSIZE];

	while (offset < end) {
		size_t blockSize = BLOCKSIZE;
		if (offset+blockSize > end)
			blockSize = end - offset;
		size_t read = blockSize;

		byte* buf = _file.read(offset, read, BUF);
		if (read != blockSize)
			throw ios_base::failure("Unexpected read error");

		_hasher.setData(offset/BLOCKSIZE, buf, read);

		offset += blockSize;
	}
}

size_t Asset::write(uint64_t offset, const void* buf, size_t size)
{
	// TODO
	return 0;
}
