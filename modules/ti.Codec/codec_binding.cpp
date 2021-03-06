/**
* This file has been modified from its orginal sources.
*
* Copyright (c) 2012 Software in the Public Interest Inc (SPI)
* Copyright (c) 2012 David Pratt
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
***
* Copyright (c) 2008-2012 Appcelerator Inc.
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#include <kroll/kroll.h>
#include "codec_binding.h"

#include <sstream>
#include <Poco/Base64Encoder.h>
#include <Poco/Base64Decoder.h>
#include <Poco/DigestEngine.h>
#include <Poco/MD2Engine.h>
#include <Poco/MD4Engine.h>
#include <Poco/MD5Engine.h>
#include <Poco/SHA1Engine.h>
#include <Poco/HMACEngine.h>
#include <Poco/HexBinaryEncoder.h>
#include <Poco/HexBinaryDecoder.h>
#include <Poco/Checksum.h>
#include <Poco/Data/BLOB.h>
#include <Poco/Data/BLOBStream.h>
#include <Poco/File.h>

#define CODEC_MD2       1
#define CODEC_MD4       2
#define CODEC_MD5       3
#define CODEC_SHA1      4
#define CODEC_CRC32     1
#define CODEC_ADLER32   2

namespace ti
{
    CodecBinding::CodecBinding(KObjectRef global) :
        StaticBoundObject("Codec"),
        global(global)
    {
        /**
         * @tiapi(method=True,name=Codec.encodeBase64,since=0.7) encode a string or Bytes into base64
         * @tiarg(for=Codec.encodeBase64,name=data,type=String|Bytes) data to encode
         * @tiresult(for=Codec.encodeBase64,type=string) returns base64 encoded string
         */
        this->SetMethod("encodeBase64", &CodecBinding::EncodeBase64);

        /**
         * @tiapi(method=True,name=Codec.decodeBase64,since=0.7) decode a string from base64
         * @tiarg(for=Codec.decodeBase64,name=data,type=String) data to decode
         * @tiresult(for=Codec.decodeBase64,type=string) returns base64 decoded string
         */
        this->SetMethod("decodeBase64", &CodecBinding::DecodeBase64);

        /**
         * @tiapi(method=True,name=Codec.digestToHex,since=0.7) encode a string or Bytes using a digest algorithm
         * @tiarg(for=Codec.digestToHex,name=type,type=int) encoding type: currently supports MD2, MD4, MD5, SHA1
         * @tiarg(for=Codec.digestToHex,name=data,type=String|Bytes) data to encode
         * @tiresult(for=Codec.digestToHex,type=string) returns encoded string
         */
        this->SetMethod("digestToHex", &CodecBinding::DigestToHex);

        /**
         * @tiapi(method=True,name=Codec.digestHMACToHex,since=0.7) digest a encoded string in HMAC
         * @tiarg(for=Codec.digestHMACToHex,name=type,type=int) encoding type: currently supports MD2, MD4, MD5, SHA1
         * @tiarg(for=Codec.digestHMACToHex,name=data,type=String) data to encode
         * @tiarg(for=Codec.digestHMACToHex,name=data,type=String) key to us for HMAC
         * @tiresult(for=Codec.digestHMACToHex,type=string) returns base64 decoded string
         */
        this->SetMethod("digestHMACToHex", &CodecBinding::DigestHMACToHex);

        /**
         * @tiapi(method=True,name=Codec.encodeHexBinary,since=0.7) encode a string or Bytes into hex binary
         * @tiarg(for=Codec.encodeHexBinary,name=data,type=String|Bytes) data to encode
         * @tiresult(for=Codec.encodeHexBinary,type=string) returns hex binary encoded string
         */
        this->SetMethod("encodeHexBinary", &CodecBinding::EncodeHexBinary);

        /**
         * @tiapi(method=True,name=Codec.decodeHexBinary,since=0.7) decode a string from hex binary
         * @tiarg(for=Codec.decodeHexBinary,name=data,type=String) data to decode
         * @tiresult(for=Codec.decodeHexBinary,type=string) returns unencoded hex binary string
         */
        this->SetMethod("decodeHexBinary", &CodecBinding::DecodeHexBinary);

        /**
         * @tiapi(method=True,name=Codec.checksum,since=0.7) compute checksum
         * @tiarg(for=Codec.checksum,name=data,type=String) data to checksum (as string)
         * @tiarg(for=Codec.checksum,name=type,type=int,optional=True) checksum type: currently supports CRC32 (default) and ADLER32
         * @tiresult(for=Codec.checksum,type=int) return checksum value
         */
        this->SetMethod("checksum", &CodecBinding::Checksum);
        
        /**
         * @tiapi(method=True,name=Codec.createZip,since=0.7) Asynchronously write the contents of a directory to a zip file
         * All files will be recursively added from the directory, and the directory will be considered the logical "root" of the zip.
         * @tiarg[Filesystem.File|String, directory] A directory root to write to the zip stream
         * @tiarg[Filesystem.File|String, zipFile] The destination zip file
         * @tiarg[Function, onComplete] A function callback that receives the zip file when writing is finished: function onComplete(destFile) {}
         */
        this->SetMethod("createZip", &CodecBinding::CreateZip);

        this->SetMethod("extractZip", &CodecBinding::ExtractZip);
        
        /**
         * @tiapi(property=True,name=Codec.MD2,since=0.7) MD2 property
         */
        this->SetInt("MD2", CODEC_MD2);
        /**
         * @tiapi(property=True,name=Codec.MD4,since=0.7) MD4 property
         */
        this->SetInt("MD4", CODEC_MD4);
        /**
         * @tiapi(property=True,name=Codec.MD5,since=0.7) MD5 property
         */
        this->SetInt("MD5", CODEC_MD5);
        /**
         * @tiapi(property=True,name=Codec.SHA1,since=0.7) SHA1 property
         */
        this->SetInt("SHA1", CODEC_SHA1);
        /**
         * @tiapi(property=True,name=Codec.CRC32,since=0.7) CRC32 property
         */
        this->SetInt("CRC32", CODEC_CRC32);
        /**
         * @tiapi(property=True,name=Codec.ADLER32,since=0.7) ADLER32 property
         */
        this->SetInt("ADLER32", CODEC_ADLER32);
    }
    
    CodecBinding::~CodecBinding()
    {
    }
    
    static std::string& GetStringFromValue(KValueRef value)
    {
        static std::string data;
        if (value->IsString())
        {
            data = value->ToString();
        }
        else
        {
            AutoPtr<Bytes> bytes(value->ToObject().cast<Bytes>());
            if (!bytes.isNull())
            {
                data = std::string(bytes->Pointer(), bytes->Length());
            }
            else
            {
                data = "";
            }
        }
        return data;
    }
    
    void CodecBinding::EncodeBase64(const ValueList& args, KValueRef result)
    {
        args.VerifyException("encodeBase64", "s|o");
        
        std::ostringstream str;
        Poco::Base64Encoder encoder(str);
        encoder << GetStringFromValue(args.at(0));
        encoder.close();
        std::string encoded = str.str();
        result->SetString(encoded);
    }

    void CodecBinding::DecodeBase64(const ValueList& args, KValueRef result)
    {
        args.VerifyException("decodeBase64", "s");

        std::string encoded = args.at(0)->ToString();
        std::stringstream str;
        str << encoded;
        Poco::Base64Decoder decoder(str);
        std::string decoded;
        decoder >> decoded;
        result->SetString(decoded);
    }

    void CodecBinding::DigestToHex(const ValueList& args, KValueRef result)
    {
        args.VerifyException("digestToHex", "i s|o");
        
        int type = args.at(0)->ToInt();
        
        Poco::DigestEngine *engine = NULL;
        
        switch(type)
        {
            case CODEC_MD2:
            {
                engine = new Poco::MD2Engine(); 
                break;
            }
            case CODEC_MD4:
            {
                engine = new Poco::MD4Engine(); 
                break;
            }
            case CODEC_MD5:
            {
                engine = new Poco::MD5Engine(); 
                break;
            }
            case CODEC_SHA1:
            {
                engine = new Poco::SHA1Engine(); 
                break;
            }
            default:
            {
                std::ostringstream msg("Unsupported encoding type: ");
                msg << type;
                throw ValueException::FromString(msg.str());
            }
        }
        
        if (args.at(1)->IsString())
        {
            engine->update(args.GetString(1));
        }
        else
        {
            AutoPtr<Bytes> bytes(args.GetObject(1).cast<Bytes>());
            if (!bytes.isNull())
            {
                engine->update(bytes->Pointer(), bytes->Length());
            }
        }
        std::string data = Poco::DigestEngine::digestToHex(engine->digest()); 
        result->SetString(data);
        delete engine;
    }

    void CodecBinding::DigestHMACToHex(const ValueList& args, KValueRef result)
    {
        args.VerifyException("digestHMACToHex", "i s s");
        
        int type = args.at(0)->ToInt();
        std::string encoded = args.at(1)->ToString();
        std::string key = args.at(2)->ToString();

        switch(type)
        {
            case CODEC_MD2:
            {
                Poco::HMACEngine<Poco::MD2Engine> engine(key); 
                engine.update(encoded); 
                std::string data = Poco::DigestEngine::digestToHex(engine.digest()); 
                result->SetString(data);
                break;
            }
            case CODEC_MD4:
            {
                Poco::HMACEngine<Poco::MD4Engine> engine(key); 
                engine.update(encoded); 
                std::string data = Poco::DigestEngine::digestToHex(engine.digest()); 
                result->SetString(data);
                break;
            }
            case CODEC_MD5:
            {
                Poco::HMACEngine<Poco::MD5Engine> engine(key); 
                engine.update(encoded); 
                std::string data = Poco::DigestEngine::digestToHex(engine.digest()); 
                result->SetString(data);
                break;
            }
            case CODEC_SHA1:
            {
                Poco::HMACEngine<Poco::SHA1Engine> engine(key); 
                engine.update(encoded); 
                std::string data = Poco::DigestEngine::digestToHex(engine.digest()); 
                result->SetString(data);
                break;
            }
            default:
            {
                std::ostringstream msg("Unsupported encoding type: ");
                msg << type;
                throw ValueException::FromString(msg.str());
            }
        }
    }

    void CodecBinding::EncodeHexBinary(const ValueList& args, KValueRef result)
    {
        args.VerifyException("encodeHexBinary", "s|o");
        
        std::stringstream str;
        Poco::HexBinaryEncoder encoder(str);
        encoder << GetStringFromValue(args.at(0));
        encoder.close();
        std::string encoded = str.str();
        result->SetString(encoded);
    }

    void CodecBinding::DecodeHexBinary(const ValueList& args, KValueRef result)
    {
        args.VerifyException("decodeHexBinary", "s");
        
        std::string encoded = args.at(0)->ToString();
        std::stringstream str;
        str << encoded;
        Poco::HexBinaryDecoder decoder(str);
        std::string decoded;
        decoder >> decoded;
        result->SetString(decoded);
    }

    void CodecBinding::Checksum(const ValueList& args, KValueRef result)
    {
        args.VerifyException("checksum", "s|o ?i");

        int type = CODEC_CRC32;
        
        if (args.size() == 2)
        {
            type = args.at(1)->ToInt();
        }
        
        Poco::Checksum *checksum = NULL;
        
        switch(type)
        {
            case CODEC_CRC32:
            {
                checksum = new Poco::Checksum(Poco::Checksum::TYPE_CRC32);
                break;
            }
            case CODEC_ADLER32:
            {
                checksum = new Poco::Checksum(Poco::Checksum::TYPE_ADLER32);
                break;
            }
            default:
            {
                std::ostringstream msg("Unsupported type: ");
                msg << type;
                throw ValueException::FromString(msg.str());
            }
        }

        if (args.at(0)->IsString())
        {
            std::string encoded = args.at(0)->ToString();
            const char *data = encoded.c_str();
            checksum->update(data,encoded.size());
            result->SetInt(checksum->checksum());
        }
        else if (args.at(0)->IsObject())
        {
            BytesRef bytes = args.at(0)->ToObject().cast<Bytes>();
            if (bytes.isNull())
            {
                delete checksum;
                throw ValueException::FromString("unsupported data type passed as argument 1");
            }
            checksum->update(bytes->Pointer(),bytes->Length());
            result->SetInt(checksum->checksum());
        }
        else
        {
            delete checksum;
            throw ValueException::FromString("unsupported data type passed as argument 1");
        }
        
        delete checksum;
    }
    
    static std::string GetPathFromValue(KValueRef value)
    {
        if (value->IsObject())
        {
            return value->ToObject()->DisplayString()->c_str();
        }
        else
        {
            return value->ToString();
        }
    }
    
    void CodecBinding::CreateZip(const ValueList& args, KValueRef result)
    {
        args.VerifyException("createZip", "s|o s|o ?m");
        
        std::string directory = GetPathFromValue(args.at(0));
        std::string zipFile = GetPathFromValue(args.at(1));
        
        if (directory.size() <= 0)
        {
            throw ValueException::FromString("Error: Directory name in createZip is empty");
        }
        if (zipFile.size() <= 0)
        {
            throw ValueException::FromString("Error: Destination file name in createZip is empty");
        }
        
        Poco::Path path(directory);
        path.makeDirectory();
        if (!Poco::File(path).exists())
        {
            throw ValueException::FromFormat("Error: Directory %s doesn't exist in createZip", directory.c_str());
        }
        
        KMethodRef zipAsyncMethod = new KFunctionPtrMethod(&CodecBinding::CreateZipAsync);
        ValueList zipArgs;
        zipArgs.push_back(Value::NewString(directory));
        zipArgs.push_back(Value::NewString(zipFile));

        AutoPtr<AsyncJob> zipJob = new AsyncJob(zipAsyncMethod);
        zipArgs.push_back(Value::NewObject(zipJob));
        if (args.size() > 2 && args.at(2)->IsMethod())
        {
            zipArgs.push_back(args.at(2));
        }

        zipJob->SetArguments(zipArgs);
        zipJob->RunAsynchronously();
        result->SetObject(zipJob);
    }

    void CodecBinding::ExtractZip(const ValueList& args, KValueRef result)
    {
        args.VerifyException("extractZip", "s|o s|o ?m");

        std::string zipFile = GetPathFromValue(args.at(0));
        std::string directory = GetPathFromValue(args.at(1));

        if (zipFile.size() <= 0)
        {
            throw ValueException::FromString("Error: Zip file name in extractZip is empty");
        }
        if (zipFile.size() <= 0)
        {
            throw ValueException::FromString("Error: Destination directory name in extractZip is empty");
        }

        KMethodRef extractAsyncMethod = new KFunctionPtrMethod(&CodecBinding::ExtractZipAsync);
        ValueList extractArgs;
        extractArgs.push_back(Value::NewString(zipFile));
        extractArgs.push_back(Value::NewString(directory));

        AutoPtr<AsyncJob> extractJob = new AsyncJob(extractAsyncMethod);
        extractArgs.push_back(Value::NewObject(extractJob));
        if (args.size() > 2)
        {
            extractArgs.push_back(args.at(2));
        }

        extractJob->SetArguments(extractArgs);
        extractJob->RunAsynchronously();
        result->SetObject(extractJob);
    }

    /*static*/
    KValueRef CodecBinding::CreateZipAsync(const ValueList& args)
    {
        std::string directory = args.GetString(0);
        std::string zipFile = args.GetString(1);
        AutoPtr<AsyncJob> job = args.GetObject(2).cast<AsyncJob>();
        KMethodRef callback = 0;
        if (args.size() > 3)
        {
            callback = args.GetMethod(3);
        }
        
        Poco::Path path(directory);
        path.makeDirectory();
        
        std::ofstream stream(UTF8ToSystem(zipFile).c_str(),
            std::ios::binary | std::ios::trunc);
        Poco::Zip::Compress compressor(stream, true);
        try
        {
            compressor.addRecursive(path);
        }
        catch (std::exception& e)
        {
            Logger::Get("Codec")->Error("exception compressing: %s", e.what());
            throw ValueException::FromFormat("Exception during zip: %s", e.what());
        }
        
        compressor.close();
        stream.close();
        
        if (!callback.isNull())
        {
            ValueList args;
            args.push_back(Value::NewString(zipFile));
            RunOnMainThread(callback, args, true);
        }
        
        return Value::Undefined;
    }

    /*static*/
    KValueRef CodecBinding::ExtractZipAsync(const ValueList& args)
    {
        std::string zipFile = args.GetString(0);
        std::string directory = args.GetString(1);
        AutoPtr<AsyncJob> job = args.GetObject(2).cast<AsyncJob>();
        KMethodRef callback = 0;
        if (args.size() > 3)
        {
            callback = args.GetMethod(3);
        }

        std::ifstream stream(UTF8ToSystem(zipFile).c_str(), std::ios::binary);
        Poco::Zip::Decompress decompressor(stream, directory);
        try
        {
            decompressor.decompressAllFiles();
        }
        catch (std::exception& e)
        {
            Logger::Get("Codec")->Error("exception decompressing: %s", e.what());
            throw ValueException::FromFormat("Exception during extraction: %s", e.what());
        }

        stream.close();

        if (!callback.isNull())
        {
            ValueList args;
            args.push_back(Value::NewString(directory));
            RunOnMainThread(callback, args, true);
        }

        return Value::Undefined;
    }
}