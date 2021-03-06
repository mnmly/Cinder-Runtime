#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "runtime/Memory.h"

#include "Test.h"
#include "HeaderOnlyTest.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CinderCompilerApp : public App {
 public:
	CinderCompilerApp();
	void draw() override;

	rt::shared_ptr<Test> mTest;
	rt::shared_ptr<HeaderOnlyTest> mHeaderOnly;
};

// (compiler) Debug Information Format: Program Database /Zi	
// (linker) /INCREMENTAL:NO as we're changing the .pdb file before building there's no need for this (performing full link anyway)
CinderCompilerApp::CinderCompilerApp()
{
	mTest = rt::make_shared<Test>();
	mHeaderOnly = rt::make_shared<HeaderOnlyTest>();
}

void CinderCompilerApp::draw()
{
	if( ! mTest ) {
		gl::clear( Color( 0, 0, 0 ) ); 
	}
	else if( mTest ){
		mTest->clear();
		mTest->render();
	}

	if( mHeaderOnly ) {
		mHeaderOnly->render();
	}
}

CINDER_APP( CinderCompilerApp, RendererGl )