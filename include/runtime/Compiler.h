/*
 Copyright (c) 2016, Simon Geilfus
 All rights reserved.
 
 This code is designed for use with the Cinder C++ library, http://libcinder.org
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:
    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include <memory>
#include "cinder/Exception.h"
#include "cinder/Filesystem.h"
#include "cinder/signals.h"

namespace runtime {

using CompilerRef = std::shared_ptr<class Compiler>;
using CompilerWeakPtr = std::weak_ptr<class Compiler>;

class CompilationResult;

class Compiler {
public:
	//! Returns a ref-counted Compiler object
	static CompilerRef create();
	//! Constructs a Compiler object
	Compiler();

	//! Describes the list of Options and arguments available when building a file
	class Options {
	public:
		//! Constructs the default Compiler's Options with the same settings as the parent project.
		Options();
		//! Specifies the path to the precompiled header.
		Options& precompiledHeader( const ci::fs::path &path, bool create );
		//! Overrides the set of compiler arguments. Will use the parent project arguments by default.
		Options& compilerArgs( const std::string &args );
		//! Overrides the set of linker arguments. Will use the parent project arguments by default.
		Options& linkArgs( const std::string &args );
		//! Enables verbose mode. Disabled by default.
		Options& verbose( bool enabled = true );
		//! Adds a preprocessor definition to the compiler options
		Options& preprocessorDef( const std::string &definition );
		//! Adds an extra include folder to the compiler options
		Options& include( const std::string &path );
		//! Adds an extra include folder to the compiler options
		Options& include( const ci::fs::path &path );
		//! Specifies additional files to be compiled (and linked).
		Options& additionalCompileList( const std::vector<ci::fs::path> &cppFiles );
		//! Specifies additional files to be linked.
		Options& additionalLinkList( const std::vector<ci::fs::path> &objFiles );
		//! Sets the output path
		Options& outputPath( const ci::fs::path &path );
		//! Enables symbol dumping with dumpbin /EXPORTS
		Options& dumpSymbols( bool dump = true );
		//! Adds a forced include as the first lined of the compiled file
		Options& forceInclude( const std::string &filename );
		//! Links the .obj generated by the app. Default to true
		Options& linkAppObjs( bool link );

	protected:
		friend class Compiler;
		bool mVerbose;
		bool mCreatePrecompiledHeader;
		bool mDumpSymbols;
		bool mLinkAppObjs;
		ci::fs::path mPrecompiledHeader;
		ci::fs::path mOutputPath;
		std::string	mCompileArgs;
		std::string	mLinkArgs;
		std::vector<ci::fs::path>	mCompileList;
		std::vector<ci::fs::path>	mLinkList;
		std::vector<std::string>	mPpDefinitions;
		std::vector<std::string>	mIncludes;
		std::vector<std::string>	mForcedIncludes;
	};
	
	//! Compiles and links the file at path. A callback can be specified to get the compilation results.
	void build( const ci::fs::path &path, const std::function<void(const CompilationResult&)> &onBuildFinish = nullptr );
	//! Compiles and links the file at path. A callback can be specified to get the compilation results.
	void build( const ci::fs::path &path, const Options &options, const std::function<void(const CompilationResult&)> &onBuildFinish = nullptr );
	
	~Compiler();
protected:
	void initializeProcess();
	void findAppBuildArguments();
	void parseProcessOutput();
	ci::fs::path resolvePath( const ci::fs::path &path );
	using ProcessPtr = std::unique_ptr<class Process>;

	ProcessPtr		mProcess;
	std::string		mCompileArgs;
	std::string		mLinkArgs;
	std::string		mProjectName;
	ci::fs::path	mAppPath;
	ci::fs::path	mProjectPath;
	ci::fs::path	mCurrentOutputPath;
	ci::fs::path	mFilePath;
	bool			mVerbose;
	std::function<void(CompilationResult&)> mBuildFinish;
	std::vector<ci::fs::path>				mTemporaryFiles;
	std::vector<std::string>				mErrors;
	std::vector<std::string>				mWarnings;
	std::vector<std::string>				mAppLinkedObjs;
	std::map<std::string,std::string>		mSymbols;
	ci::signals::Connection					mProcessOutputConnection;
};

class CompilationResult {
public:
	//! Returns the path of the file that has been compiled
	ci::fs::path getFilePath() const;
	//! Returns the path of the compilation output
	ci::fs::path getOutputPath() const;
	//! Returns whether the compilation ended with errors
	bool hasErrors() const;
	//! Returns the list of errors 
	const std::vector<std::string>& getErrors() const;
	//! Returns the list of warnings
	const std::vector<std::string>& getWarnings() const;
	//! Returns a map of exported symbols and their mangled names
	const std::map<std::string,std::string>& getSymbols() const;
	
	CompilationResult( const ci::fs::path &filePath, const ci::fs::path &outputPath, const std::vector<std::string>& errors, const std::vector<std::string>& warnings, const std::map<std::string,std::string> &symbols );
protected:
	ci::fs::path mOutputPath;
	ci::fs::path mFilePath;
	std::vector<std::string> mErrors;
	std::vector<std::string> mWarnings;
	std::map<std::string,std::string> mSymbols;
};

class CompilerException : public ci::Exception {
public:
	CompilerException( const std::string &message ) : ci::Exception( message ) {}
};

} // namespace runtime

namespace rt = runtime;