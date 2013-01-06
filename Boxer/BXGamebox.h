/* 
 Boxer is copyright 2011 Alun Bestor and contributors.
 Boxer is released under the GNU General Public License 2.0. A full copy of this license can be
 found in this XCode project at Resources/English.lproj/BoxerHelp/pages/legalese.html, or read
 online at [http://www.gnu.org/licenses/gpl-2.0.txt].
 */


//BXGamebox represents a single Boxer gamebox and offers methods for retrieving and persisting
//bundled drives, configuration files and documentation. It is based on NSBundle but does not
//require that Boxer gameboxes use any standard OS X bundle folder structure.
//(and indeed, gameboxes with an OS X bundle structure haven't been tested.)

//TODO: it is inappropriate to subclass NSBundle for representing a modifiable file package,
//and we should instead be using an NSFileWrapper directory wrapper.

#import <Cocoa/Cocoa.h>
#import "BXUndoExtensions.h"


#pragma mark Gamebox-related error constants

extern NSString * const BXGameboxErrorDomain;
enum {
	BXTargetPathOutsideGameboxError
};


#pragma mark -
#pragma mark Game Info plist constants

//The gameInfo key under which we store the game's identifier.
//Will be an NSString.
extern NSString * const BXGameIdentifierGameInfoKey;

//The gameInfo key under which we store the type of the game's identifier.
//Will be an NSNumber of BXGameIdentifierTypes.
extern NSString * const BXGameIdentifierTypeGameInfoKey;

//The gameInfo key under which we store the default program path,
//relative to the base folder of the gamebox.
extern NSString * const BXTargetProgramGameInfoKey;

//The gameInfo key under which we store an array of launcher shortcuts.
//Each entry in the array is an NSDictionary whose keys are listed under
//"Launcher dictionary constants".
extern NSString * const BXLaunchersGameInfoKey;

//The gameInfo key under which we store the close-on-exit toggle flag as an NSNumber.
extern NSString * const BXCloseOnExitGameInfoKey;


#pragma mark -
#pragma mark Launcher dictionary constants.

//The display name for the launcher item.
extern NSString * const BXLauncherTitleKey;

//The path of the program for the launcher, relative to the base folder of the gamebox.
extern NSString * const BXLauncherPathKey;

//Launch-time parameters to pass to the launched program at startup.
extern NSString * const BXLauncherArgsKey;

//Whether this is the default launcher for this gamebox
//(i.e. the launcher that will be executed when the gamebox is first launched.)
extern NSString * const BXLauncherIsDefaultKey;


#pragma mark -
#pragma mark Filename constants

//The filename of the symlink pointing to the gamebox's target executable.
//No longer used.
extern NSString * const BXTargetSymlinkName;

//The filename and extension of the gamebox configuration file.
extern NSString * const BXConfigurationFileName;
extern NSString * const BXConfigurationFileExtension;

//The filename and extension of the game info manifest inside the gamebox.
extern NSString * const BXGameInfoFileName;
extern NSString * const BXGameInfoFileExtension;

//The filename of the documentation folder inside the gamebox.
extern NSString * const BXDocumentationFolderName;


//The different kinds of game identifiers we can have.
enum {
	BXGameIdentifierUserSpecified	= 0,	//Manually specified type.
	BXGameIdentifierUUID			= 1,	//Standard UUID. Generated for empty gameboxes.
	BXGameIdentifierEXEDigest		= 2,	//SHA1 digest of each EXE file in the gamebox.
	BXGameIdentifierReverseDNS		= 3,	//Reverse-DNS (net.washboardabs.boxer)-style identifer.
};
typedef NSUInteger BXGameIdentifierType;


#pragma mark -
#pragma mark Interface

@interface BXGamebox : NSBundle <BXUndoable>
{
	NSMutableDictionary *_gameInfo;
    NSMutableArray *_launchers;
    id <BXUndoDelegate> _undoDelegate;
}

#pragma mark -
#pragma mark Properties

//Returns a dictionary of gamebox metadata loaded from Boxer.plist.
//Keys in this can also be retrieved with objectForInfoDictionaryKey: and set with setObjectForInfoDictionaryKey:
//(They cannot be set directly on gameInfo.)
@property (readonly, retain, nonatomic) NSDictionary *gameInfo;

//The path to the DOS game's base folder. Currently this is equal to [NSBundle bundlePath].
@property (readonly, nonatomic) NSString *gamePath;

//The name of the game, suitable for display. This is the gamebox's filename minus any ".boxer" extension.
@property (readonly, nonatomic) NSString *gameName;

//The unique identifier of this game.
@property (copy, nonatomic) NSString *gameIdentifier;

//An array of absolute file paths to DOS executables found inside the gamebox.
@property (readonly, nonatomic) NSArray *executables;

//Arrays of paths to additional DOS drives discovered within the package.
@property (readonly, nonatomic) NSArray *hddVolumes;
@property (readonly, nonatomic) NSArray *cdVolumes;
@property (readonly, nonatomic) NSArray *floppyVolumes;

//Returns the path at which the configuration file is located (or would be, if it doesn’t exist.)
@property (readonly, nonatomic) NSString *configurationFilePath;

//The path to the DOSBox configuration file for this package. Will be nil if one does not exist.
@property (readonly, nonatomic) NSString *configurationFile;

//The path to the default executable for this gamebox. Will be nil if the gamebox has no target executable.
@property (copy, nonatomic) NSString *targetPath;

//Whether the emulation should finish once the target program exits, rather than returning to the DOS prompt.
@property (assign, nonatomic) BOOL closeOnExit;

//The cover art image for this gamebox. Will be nil if the gamebox has no custom cover art.
//This is stored internally as the gamebox's OS X icon resource.
@property (copy, nonatomic) NSImage *coverArt;

//Program launchers for this gamebox, displayed as favorites in the launch panel.
@property (readonly, nonatomic) NSArray *launchers;

//The default launcher for this gamebox, which should be launched the first time the gamebox is run.
//This will be nil if the gamebox has no default launcher.
@property (readonly, nonatomic) NSDictionary *defaultLauncher;

//The index in the launchers array of the default launcher.
@property (assign, nonatomic) NSInteger defaultLauncherIndex;

//The delegate from whom we will request an undo manager for undoable operations.
@property (assign, nonatomic) id <BXUndoDelegate> undoDelegate;


#pragma mark -
#pragma mark Class methods

//Re-casts the return value as a BXGamebox instead of an NSBundle
+ (BXGamebox *)bundleWithPath: (NSString *)path;

//Filename patterns for executables to exclude from searches.
+ (NSSet *) executableExclusions;

#pragma mark -
#pragma mark Instance methods

//Returns whether the specified path is valid to be the default target of this gamebox
- (BOOL) validateTargetPath: (id *)ioValue error: (NSError **)outError;

//Retrieve all volumes matching the specified filetypes.
- (NSArray *) volumesOfTypes: (NSSet *)fileTypes;

//Get/set metadata in the gameInfo dictionary.
- (id) gameInfoForKey: (NSString *)key;
- (void) setGameInfo: (id)info forKey: (NSString *)key;

//Clear resource caches for documentation, gameInfo and executables.
- (void) refresh;


- (void) addLauncher: (NSDictionary *)launcher;
- (void) insertLauncher: (NSDictionary *)launcher atIndex: (NSUInteger)index;

//Insert new launchers into the launcher array.
- (void) insertLauncherWithTitle: (NSString *)title
                            path: (NSString *)path
                       arguments: (NSString *)launchArguments
                         atIndex: (NSUInteger)index;


- (void) addLauncherWithTitle: (NSString *)title
                         path: (NSString *)path
                    arguments: (NSString *)launchArguments;

//Remove the specified launcher data from the launchers array.
- (void) removeLauncher: (NSDictionary *)launcher;

- (void) removeLauncherAtIndex: (NSUInteger)index;

@end


typedef enum {
    BXGameboxDocumentationRename,
    BXGameboxDocumentationReplace,
} BXGameboxDocumentationConflictBehaviour;

@interface BXGamebox (BXGameDocumentation)

#pragma mark - Documentation autodiscovery

//Returns an array of documentation found in the gamebox. If the gamebox has a documentation
//folder, the contents of this folder will be returned; otherwise, the rest of the gamebox
//will be searched for documentation.
@property (readonly, nonatomic) NSArray *documentationURLs;

//Returns whether the gamebox has a documentation folder of its own.
//If not, this can be created with populateDocumentationFolderWithError:.
@property (readonly, nonatomic) BOOL hasDocumentationFolder;


+ (NSSet *) documentationTypes;			//UTIs recognised as documentation files.
+ (NSSet *) documentationExclusions;	//Filename patterns for documentation to exclude from searches.

//Returns all the documentation files in the specified filesystem location.
+ (NSArray *) URLsForDocumentationInLocation: (NSURL *)location searchSubdirectories: (BOOL)searchSubdirs;

//Returns whether the file at the specified URL appears to be documentation.
+ (BOOL) isDocumentationFileAtURL: (NSURL *)URL;

//Returns the URL for the documentation folder of this gamebox, if one exists.
//If createIfMissing is YES, this folder will be created if it does not exist and will
//be automatically populated with symlinks to documentation found elsewhere in the gamebox.
//In the event that the folder could not be created, this will return nil and populate outError.
//If createIfMissing is NO and the folder does not exist or is inaccessible, this will return nil
//and outError will be populated with the reason the folder could not be accessed.
- (NSURL *) documentationFolderURLCreatingIfMissing: (BOOL)createIfMissing error: (out NSError **)outError;

//Populates the documentation folder with symlinks to documentation found elsewhere in the gamebox.
//Note that this will not create the documentation folder if it does not already exist.
- (BOOL) populateDocumentationFolderWithError: (out NSError **)outError;


//Copies the file at the specified location into the documentation folder,
//creating the folder first if it is missing.
//If title is specified, it will be used as the filename for the imported file;
//otherwise, the file's original name will be used.
//In the event of a filename collision, conflictBehaviour determines whether
//the file will be replaced or renamed (by appending a number to the filename).
//Returns the URL of the imported file on success, or nil and populates outError on failure.
- (NSURL *) addDocumentationFileFromURL: (NSURL *)sourceURL
                              withTitle: (NSString *)title
                               ifExists: (BXGameboxDocumentationConflictBehaviour)conflictBehaviour
                                  error: (out NSError **)outError;

//Adds a symlink to the specified URL into the gamebox's documentation folder,
//creating the folder first if it is missing.
//If title is specificied, it will be used as the filename for the imported file;
//otherwise, the file's original name will be used.
//In the event of a filename collision, conflictBehaviour determines whether
//the file will be replaced or renamed (by appending a number to the filename).
//Returns the URL of the symlink on success, or nil and populates outError on failure.
- (NSURL *) addDocumentationSymlinkToURL: (NSURL *)sourceURL
                               withTitle: (NSString *)title
                                ifExists: (BXGameboxDocumentationConflictBehaviour)conflictBehaviour
                                   error: (out NSError **)outError;

//Moves the documentation file at the specified URL to the trash.
//Will fail and do nothing if the specified URL is not located within the gamebox's documentation folder.
//Returns the URL of the item's new location in the trash  on success, or nil and populates outError on failure.
- (NSURL *) trashDocumentationURL: (NSURL *)documentationURL error: (out NSError **)outError;

//Returns whether the specified documentation file can be removed from the gamebox.
//Will return NO if the URL is not located within the documentation folder.
- (BOOL) canTrashDocumentationURL: (NSURL *)documentationURL;
@end