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


#ifndef LINKEDASSETSTORE_HPP
#define LINKEDASSETSTORE_HPP

#include <boost/asio/io_service.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/function.hpp>

#include "asset.hpp"
#include "bithorde.pb.h"
#include "../lib/threadpool.hpp"

typedef google::protobuf::RepeatedPtrField< bithorde::Identifier > BitHordeIds;

class LinkedAssetStore
{
public:
	typedef boost::function< void ( Asset::Ptr )> ResultHandler;
	
	LinkedAssetStore(boost::asio::io_service& ioSvc, const boost::filesystem::path& baseDir);

	/**
	 * Add an asset to the idx, creating a hash in the background. When hashing is done,
	 * responder will be called asynchronously.
	 *
	 * In responder, the shared_ptr will be set to either a readily hashed instance, or empty if hashing failed
	 */
	void addAsset(const boost::filesystem::path& file, ResultHandler handler);

	/**
	 * Finds an asset by bithorde HashId. (Only the tiger-hash is actually used)
	 */
	Asset::Ptr findAsset(const BitHordeIds& ids);

private:
	void _addAsset(Asset::Ptr& asset, ResultHandler upstream);

	ThreadPool _threadPool;
	boost::asio::io_service& _ioSvc;
	boost::filesystem::path _baseDir;
	boost::filesystem::path _metaFolder;
	boost::filesystem::path _tigerFolder;
};

#endif // LINKEDASSETSTORE_HPP
