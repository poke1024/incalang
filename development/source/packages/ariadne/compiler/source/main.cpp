// ===========================================================================
//	main.cp											 ©2001-2001 Bernhard Liebl
// ===========================================================================

#include "run.h"

#if TARGET_OS_MAC
#include <SIOUX.h>
#include <console.h>
#include "macmain.h"
#endif

using namespace compiler;

#if TARGET_OS_MAC

int
alt_main(
	int					argc,
	char*				argv[] )

#else

int
main(
	int					argc,
	char*				argv[] )

#endif

{
#if TARGET_OS_MAC
	SIOUXSettings.initializeTB = false;
	SIOUXSettings.standalone = false;
	SIOUXSettings.setupmenus = false;
	SIOUXSettings.autocloseonquit = true;
	SIOUXSettings.asktosaveonclose = false;
	
	SIOUXSettings.showstatusline = false;
	SIOUXSettings.userwindowtitle = "\pAriadne Compiler Status";
	
	//argc = ccommand( &argv );
#endif

	std::cout << "*** ariadne compiler ***\n";
	std::cout << "(C) 1999-2001 Bernhard Liebl\n";
	std::cout << "built " << __DATE__ << "\n\n";

	if( argc < 2 )
	{
#if TARGET_OS_MAC
		std::cout << "Syntax: ariadne <source file>\n";
#elif TARGET_OS_WIN32
		std::cout << "Syntax: ariadne.exe <source file>\n";
#endif
		std::cout << "Parameters:\n";
		std::cout << "\t-iPATH system include path\n";

		return EXIT_FAILURE;
	}

	compiler_t			compiler;

	for( int i = 1; i < argc; i++ )
	{
		if( argv[ i ][ 0 ] == '-' )
		{
			switch( std::tolower( argv[ i ][ 1 ] ) )
			{
				case 'i':
					compiler.m_systempath = &argv[ i ][ 2 ];
					break;
			}
		}
		else
			compiler.m_sourcefile = argv[ i ];
	}

	error_t*			error;
	
	error = compiler.run();
	
	if( !error )
	{
		std::cout << "File compiled successfully.\n";
		
		return EXIT_SUCCESS;
	}
		
	while( error )
	{
		fprintf( stderr, "%s in file '%s' in line #%lu\n",
			error->what.c_str(),
			error->file.c_str(),
			error->line );
		
		fprintf( stderr, "%s\n",
			error->portion.c_str() );
			
		error = error->next;
	}
	
	return EXIT_FAILURE;
}
