import os.path as path
import sdk
import distutils.dir_util as dir_util
from tide import BuildConfig

build = BuildConfig(
	PRODUCT_VERSION = sdk.get_titanium_version(),
	PRODUCT_NAME = 'Titanium',
	GLOBAL_NS_VARNAME = 'Titanium',
	CONFIG_FILENAME = 'tiapp.xml',
	BUILD_DIR = path.abspath('build'),
	THIRD_PARTY_DIR = path.join(path.abspath('tide'), 'thirdparty'),
	DISTRIBUTION_URL = 'api.appcelerator.net',
	CRASH_REPORT_URL = 'api.appcelerator.net/p/v1/app-crash-report'
)
EnsureSConsVersion(1,2,0)
EnsurePythonVersion(2,5)

build.set_libtide_source_dir(path.abspath('tide'))

build.titanium_source_dir = path.abspath('.')
build.titanium_sdk_dir = path.join(build.titanium_source_dir, 'sdk')

build.env.Append(CPPPATH=[
	build.titanium_source_dir,
	build.libtide_source_dir,
	build.libtide_include_dir
])

# debug build flags
debug = ARGUMENTS.get('debug', 0)
if debug:
	build.env.Append(CPPDEFINES = ('DEBUG', 1))
	if build.is_win32():
		build.env.Append(CCFLAGS=['/Z7'])  # max debug
		build.env.Append(CPPDEFINES=('WIN32_CONSOLE', 1))
	else:
		build.env.Append(CPPFLAGS=['-g'])  # debug
else:
	build.env.Append(CPPDEFINES = ('NDEBUG', 1 ))
	if not build.is_win32():
		build.env.Append(CPPFLAGS = ['-O9']) # max optimizations
if build.is_win32():
	build.env.Append(CCFLAGS=['/EHsc', '/GR', '/MD'])
	build.env.Append(LINKFLAGS=['/DEBUG', '/PDB:${TARGET}.pdb'])

Export('build', 'debug')
targets = COMMAND_LINE_TARGETS
clean = 'clean' in targets or ARGUMENTS.get('clean', 0)
build.nopackage = ARGUMENTS.get('nopackage', 0)

if clean:
	print "cleaning build directory: %s" % build.dir
	if path.exists(build.dir):
		dir_util.remove_tree(build.dir)
	Exit(0)

# forcing a crash to test crash detection
if ARGUMENTS.get('test_crash', 0):
	build.env.Append(CPPDEFINES = ('TEST_CRASH_DETECTION', 1))

## libtide *must not be required* for installation
SConscript('SConscript.thirdparty')
SConscript('installer/SConscript')

# After libtide builds, the environment will  link 
# against libtide, so anything that should not be
# linked against libtide should be above this point.
SConscript('tide/SConscript', exports='debug')
SConscript('modules/SConscript', exports='debug')
SConscript('SConscript.dist')

run = ARGUMENTS.get('run', 0)
run_with = ARGUMENTS.get('run_with', 0)

Export('run','run_with')
SConscript('tools/SConscript')
