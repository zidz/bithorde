#ifndef BITHORDE_ASSET_H
#define BITHORDE_ASSET_H

#include <inttypes.h>
#include <utility>
#include <vector>

#include <boost/bind/placeholders.hpp>
#include <boost/bind/arg.hpp>
#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>

#include "bithorde.pb.h"
#include "types.h"

namespace bithorde {

class Client;

static boost::arg<1> ASSET_ARG_STATUS;

class Asset
{
public:
	typedef boost::shared_ptr<Client> ClientPointer;
	typedef int Handle;

	explicit Asset(ClientPointer client);
	virtual ~Asset();

	bool isBound();
	uint64_t size();

	typedef boost::signals2::signal<void (const bithorde::AssetStatus&)> StatusSignal;
	typedef boost::signals2::signal<void ()> VoidSignal;
	VoidSignal closed;
	StatusSignal statusUpdate;

	void close();
protected:
	ClientPointer _client;
	Handle _handle;
	int64_t _size;

	friend class Client;
	virtual void handleMessage(const bithorde::AssetStatus &msg);
	virtual void handleMessage(const bithorde::Read::Response &msg) = 0;
};

static boost::arg<1> ASSET_ARG_OFFSET;
static boost::arg<2> ASSET_ARG_DATA;
static boost::arg<3> ASSET_ARG_TAG;

class ReadAsset : public Asset
{
public:
	typedef std::pair<bithorde::HashType, ByteArray> Identifier;
	typedef std::vector<Identifier> IdList;

	explicit ReadAsset(ClientPointer client, ReadAsset::IdList requestIds);

	int aSyncRead(uint64_t offset, ssize_t size);
	const IdList & requestIds() const;

	typedef boost::signals2::signal<void (uint64_t offset, ByteArray& data, int tag)> DataSignal;
	DataSignal dataArrived;

protected:
	virtual void handleMessage(const bithorde::AssetStatus &msg);
	virtual void handleMessage(const bithorde::Read::Response &msg);

private:
	IdList _requestIds;
};

class UploadAsset : public Asset
{
public:
	explicit UploadAsset(ClientPointer client, uint64_t size);

	bool tryWrite(uint64_t offset, byte* data, size_t amount);

protected:
	virtual void handleMessage(const bithorde::Read::Response &msg);
};

}

#endif // BITHORDE_ASSET_H
