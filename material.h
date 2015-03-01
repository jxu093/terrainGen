typedef struct material {
	float amb[4], dif[4], spec[4], emis[4], shin;
} material;

typedef struct lighting {
	float pos[4], amb[4], dif[4], spec[4];
} lighting;
