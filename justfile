# Use the preset defined in CMakePresets.json
preset := "debug"
executable := "build/FluidFlowVisualizer"
test_executable := "build/UnitTests"


# --# 1. Status Indicators (Used for logging results)
SUCCESS := "✅"  #  Success / Pass
NOTICE  := "ℹ️"   #  Info / Logging (Changed from 💡)
CAUTION := "⚠️"   #  Warning
FAILURE := "❌"  #  Error / Fail (Changed from 🚨)

# 2. Development Lifecycle (Actions)
INIT    := "⚙️"   #  Initialization / System Config
BUILD   := "🔨"  #  Compiling / Making (Changed from 🏗️)
LINK    := "🔗"  #  Linking files
RUN     := "▶️"   #  Local Execution (Changed from 🚀)
TEST    := "🧪"  #  Unit / Integration Testing
CLEAN   := "🧹"  #  Garbage Collection / Cleaning

# 3. Ops & Maintenance (Future Proofing)
PACKAGE := "📦"  #  Bundling / Packaging
LINT    := "🎨"  #  Linting / Code Style (Artist Palette is common)
DOCS    := "📝"  #  Documentation (Memo is cleaner than Books)
DEPLOY  := "🚀"  #  Shipping / Deployment
LOCK    := "🔒"  #  Security / Locking
TAG     := "🔖"  #  Version Tags- Recipes ---

# The "Magic" Command: Ensures everything is ready, then runs.
# default: test run
default: run
init: configure setup

# 1. Configure: Generates build files (Skipped efficiently by CMake if nothing changed)
configure:
    @echo {{NOTICE}} " Configuring CMake with preset {{preset}}..."
    @cmake --preset {{preset}}

# 2. Build: Compiles the code using Ninja
build:
    @echo {{BUILD}} " Building project..."
    @cmake --build --preset {{preset}} || (echo {{FAILURE}} "Build failed"; exit 1)

# 3. Setup: Links compile_commands.json so LazyVim LSP works instantly
setup:
    @echo {{LINK}} " Linking compile_commands.json..."
    @ln -sf build/compile_commands.json .

# 4. Run: Executes the binary
run: build
    @echo {{RUN}} " Running Application..."
    @./{{executable}}

# Utility: Clean everything to start fresh
clean:
    @echo {{CLEAN}} " Cleaning build artifacts..."
    @rm -rf build compile_commands.json
    @echo {{SUCCESS}} "Workspace clean."

test: build
    @echo {{TEST}}" Running Unit Tests..."
    # @./{{test_executable}}
    @ctest --preset unit-tests -V

