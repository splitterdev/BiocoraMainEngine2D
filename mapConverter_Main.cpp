
#include "src/Core.h"

#include "src/utils/Exception.h"
#include "src/utils/Vector2D.h"
#include "src/utils/MathUtils.h"
#include "src/drawing/Renderer.h"
#include "src/drawing/Texture.h"

#include "src/utils/DirectoryHelper.h"

#include "src/datamodel/utils/BlockInfo.h"
#include "src/datamodel/utils/Map.h"
#include "src/datamodel/utils/MapFromSurface.h"

string getOutputFile( string input, string extension ) {
    unsigned int lastExtSeparator = input.find_last_of( '.' );
    if ( lastExtSeparator == string::npos ) {
        return input + extension;
    } else {
        return input.substr( 0, lastExtSeparator ) + extension;
    }
}

int main( int argc, char** argv ) {

    if ( !MapFromSurface::Init( "mapConverterSettings.dat" ) ) {
        printf( "Failed to load settings file.\n" );
        return 0;
    }

    string directory = "mapConverter/";

    list < string > files = DirectoryHelper::listAllFiles( directory, "*.bmp", false );

    printf( "Conversion begin. Checking %d objects...\n", files.size() );
    for ( list < string >::iterator i = files.begin(); i != files.end(); i++ ) {
        string input = ( *i );
        string fullPath = directory + input;
        printf( " > Processing input: \"%s\"...\n", fullPath.c_str() );
        MapFromSurface::processMap( fullPath, getOutputFile( fullPath, string( ".dat" ) ) );
    }
    printf( "Conversion done.\n" );

    return 0;
}
