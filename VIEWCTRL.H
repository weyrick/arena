// View control module header file
#define PI 3.141592654


const XORIGIN=160;         // Virtual x screen origin
const YORIGIN=90;	   // Virtual y screen origin
const WIND_X=0;            // Window upper lefthand x
const WIND_Y=0;            // Window upper lefthand y
const WIND_X2=319;	   // Window lower righthand x
const WIND_Y2=170;   	   // Window lower righthand y
const FOCAL_DISTANCE=400;  // Viewer distance from screen
const GROUNDCOLOR=225;     // Ground color
const SKYCOLOR=159;	   // Sky color

void SetupGfxSystem(void);   // Sets up the graphics screen
void ShutdownGfxSystem(void);// Shuts down gfx, restores old video
void InitViewSystem(int db); // Initializes the view system
void Show_Palette(void);     // Shows little palette in corner (debug)
void UpdateView(void);       // Updates the current view
int ReturnFrames(void);      // Returns number of drawn frames
int GetAllInput(void);       // Gets all input (local/remote)-updates strcts



