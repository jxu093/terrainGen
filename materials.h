typedef struct material {
	float amb[4], dif[4], spec[4], emis[4], shin;
} material;

typedef struct lighting {
	float pos[4], amb[4], dif[4], spec[4];
} lighting;


// material redPlastic = {
// 	{0.3, 0.0, 0.0, 1.0},
// 	{0.6, 0.0, 0.0, 1.0},
// 	{0.8, 0.6, 0.6, 1.0},
// 	{0.0, 0.0, 0.0, 0.0}//,
// 	//32.0
// };

// material greenThing = {
// 	{0.5, 1, 0.5, 1},
// 	{0, 1, 0, 1},
// 	{0.5, 0.5, 0.5, 1}
// };

// material blueThing = {
// 	{0, 0, 0.6, 1},
// 	{0, 0, 1, 1},
// 	{0.1, 0.1, 1, 1}
// };

// material axis = {
// 	{1, 0, 0, 1},
// 	{0.9, 0.5, 0, 1},
// 	{0.9, 0.5, 0, 1}
// };

// lighting genericLight = {
// 	{15, 15, 5, 1},
// 	{1, 1, 1, 1},
// 	{1, 0, 0, 1},
// 	{1, 1, 1, 1}
// };