#include <stdio.h>
#include "WearhouseManager.h"


Package *create_package(long priority, const char* destination){
	// TODO 3.1

	Package* package = (Package*)malloc(sizeof(Package));
	package->priority = priority;
	
	if(destination != NULL){
		package->destination = strdup(destination);
	}
	
	return package;
}

void destroy_package(Package* package){
	// TODO: 3.1

	free(package);
}

Manifest* create_manifest_node(void){
	// TODO: 3.1

	Manifest* manifest_node = (Manifest*)malloc(sizeof(Manifest));

	manifest_node->next = NULL;
	manifest_node->prev = NULL;
	manifest_node->package = NULL;

	return manifest_node;
}

void destroy_manifest_node(Manifest* manifest_node){
	// TODO: 3.1.

	destroy_package(manifest_node->package);
	free(manifest_node);

}

Wearhouse* create_wearhouse(long capacity){
	// TODO: 3.2

	if(capacity <= 0) return NULL;

	Wearhouse* wearhouse = (Wearhouse*)malloc(sizeof(Wearhouse));
	wearhouse->packages = (Package**)malloc(capacity*sizeof(Package *));
	wearhouse->size = 0;
	wearhouse->capacity = capacity;

	return wearhouse;
}

Wearhouse *open_wearhouse(const char* file_path){
	ssize_t read_size;
	char* line = NULL;
	size_t len = 0;
	char* token = NULL;
	Wearhouse *w = NULL;


	FILE *fp = fopen(file_path, "r");
	if(fp == NULL)
		goto file_open_exception;

	if((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		w = create_wearhouse(atol(token));

		free(line);
		line = NULL;
		len = 0;
	}

	while((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		long priority = atol(token);
		token = strtok(NULL, ",\n ");
		Package *p = create_package(priority, token);
		w->packages[w->size++] = p;

		free(line);
		line = NULL;
		len = 0;
	}

	free(line);


	fclose(fp);
	return w;

	file_open_exception:
	return NULL;
}

long wearhouse_is_empty(Wearhouse *w){
	// TODO: 3.2

	if(w != NULL && w->size == 0)
		return 1;
	return 0;
}

long wearhouse_is_full(Wearhouse *w){
	// TODO: 3.2

	if((w->size == w->capacity) && (w != NULL))
		return 1;
	return 0;
}

long wearhouse_max_package_priority(Wearhouse *w){
	// TODO: 3.2

	long i,max = w->packages[0]->priority;

	for(i = 1; i < w->size; i++){
		if(w->packages[i]->priority > max)
			max = w->packages[i]->priority;
	}

	return max;
}

long wearhouse_min_package_priority(Wearhouse *w){
	// TODO: 3.2

	long i,min = w->packages[0]->priority;

	for(i = 1; i < w->size; i++){
		if(w->packages[i]->priority < min)
			min = w->packages[i]->priority;
	}

	return min;
	
}


void wearhouse_print_packages_info(Wearhouse *w){
	for(long i = 0; i < w->size; i++){
		printf("P: %ld %s\n",
				w->packages[i]->priority,
				w->packages[i]->destination);
	}
	printf("\n");
}

void destroy_wearhouse(Wearhouse* wearhouse){
	// TODO: 3.2

	long i;

	for(i = 0; i < wearhouse->size; i++){
		destroy_package(wearhouse->packages[i]);
	}

	free(wearhouse);

}


Robot* create_robot(long capacity){
	// TODO: 3.2

	if(capacity < 0) return NULL;

	Robot* robot = (Robot*)malloc(sizeof(Robot));
	
	robot->manifest = NULL;
	robot->size = 0;
	robot->capacity = capacity;
	robot->next = NULL;

	return robot;
}

int robot_is_full(Robot* robot){
	//TODO: 3.2

	if((robot->size == robot->capacity) && (robot != NULL))
		return 1;
	 return 0;
}

int robot_is_empty(Robot* robot){
	// TODO: 3.2

	if((robot->size == 0) && (robot != NULL))
		return 1;
	 return 0;
}

Package* robot_get_wearhouse_priority_package(Wearhouse *w, long priority){
	// TODO: 3.2

	if(w == NULL) return NULL;

	Wearhouse* p = w;
	long i = 0;

	//parcurg vectorul si cand gasesc un packet cu proprietatea gasita il returnez
	while(i < p->size){
		if(p->packages[i]->priority == priority)
			return p->packages[i];
		i++;
	}


	return NULL;
}

void robot_remove_wearhouse_package(Wearhouse *w, Package* package){
	// TODO: 3.2

	if(w == NULL || package == NULL || w->size == 0) return;

	long i = 0,j;
	Wearhouse* p = w;


	while(i < p->size){//se cauta pachetul
		if(p->packages[i] == package){
			for(j = i; j < p->size-1; j++){// se actualizeaza vectorul cu pachete
				p->packages[j] = p->packages[j+1];
			}
			p->size--;
			return;
		}
		
		i++;
	}

	
}

void add_before(Manifest* reference_node, Manifest* node, Robot* r){

	if(reference_node->prev == NULL){ //se verifica daca se adauga la inceputul listei
		node->next = reference_node;
		reference_node->prev = node;
		r->manifest = node;
		return;
	}

	//altfel se refac legaturile pentru adaugarea intre doua elemente,
	//in fata elementului referinta
	node->prev = reference_node->prev;
	node->next = reference_node;	
	reference_node->prev->next = node;
	reference_node->prev = node;
	return;

}

void add_after(Manifest* reference_node, Manifest* node){

	//se adauga la sfrasitul listei
	node->prev = reference_node;
	reference_node->next = node;
	return;

}


void robot_load_one_package(Robot* robot, Package* package){
	// TODO:  3.2
    

    if(robot_is_full(robot) || package == NULL || robot == NULL) return;


    Manifest* node = create_manifest_node();
	node->package = package;
	node->next = NULL;
	node->prev = NULL;

    if(robot_is_empty(robot)){ // daca robotul nu contine niciun pachet
		robot->manifest = node;
		robot->size++;
		return;
	}


	Manifest* i;

//se parcurge lista pana cand se adauga elementul sau pana cand se sfarseste lista
    for(i = robot->manifest; i != NULL; i = i->next){
        if(node->package->priority > i->package->priority){
        	//se adauga in fata elementului de la pasul i
        	add_before(i,node,robot);
            robot->size++;
            return;
        }
//daca pachetul are aceeasi prioritate cu pachetul de la pasul i,
// se itereaza cat timp sunt pachete cu aceeasi prioritate
        if(i->package->priority == node->package->priority)
            while(i->package->priority == node->package->priority){
                if(strcmp(node->package->destination, i->package->destination) < 0){
                	add_before(i,node,robot);
                    robot->size++;
                    return;
                }

                if(i->next == NULL){
                    add_after(i,node);
                    robot->size++;
                    return;
                    }

                i = i->next;
            }

        if(i->next == NULL)
            break;

    }
//daca nu s-a indeplinit nicio conditie de mai sus, 
//se adauga la sfarsit
    add_after(i,node);
    robot->size++;
    return;
}
    
long robot_load_packages(Wearhouse* wearhouse, Robot* robot){
	// TODO: 3.2

	if(wearhouse == NULL || robot == NULL) return 0;

	long loaded_package = 0,max_priority;
	Package* package;

 //se realizeaza operatia de incarcare de pachete cat timp tirul are pachete 
 //si cat timp robotul nu e plin 
	while(!wearhouse_is_empty(wearhouse) && !robot_is_full(robot)){
		max_priority = wearhouse_max_package_priority(wearhouse);
		package = robot_get_wearhouse_priority_package(wearhouse,max_priority);
		robot_load_one_package(robot,package);
		robot_remove_wearhouse_package(wearhouse,package);
		loaded_package++;
	}

	return loaded_package;

}

Package* robot_get_destination_highest_priority_package(Robot* robot, const char* destination){
	// TODO: 3.2

	if(robot == NULL) return NULL;


	Manifest* nod = robot->manifest;
	
	while(nod != NULL){
		if(strcmp(nod->package->destination, destination) == 0)
			return	nod->package;
		nod = nod->next;
	}

	return NULL;
}

void destroy_robot(Robot* robot){
	// TODO: 3.2

	Manifest* i = robot->manifest;
	Manifest* nod;

	while(i != NULL){
		nod = i;
		i = i->next;
		destroy_manifest_node(nod);
	} 

	robot->manifest = NULL;
	free(robot);

}

void delete_package(Package* p, Robot* r){
//functia sterge un pachet din robot
	
	if(r->size == 1){ // daca exista doar un pachet
		r->manifest = NULL;
		r->size--;
		return;
	}


	if(r->manifest->package == p){ //exista cel putin 2 packete si se elimina primul
		r->manifest->next->prev = NULL;
		r->manifest = r->manifest->next;
		r->size--;
		return;
	}


	Manifest* m = r->manifest;

	while(m->package != p){ // iterez catre pachet
		m = m->next;
	}

	if(m->next == NULL){//se elimina daca e ultimul
		m->prev->next = NULL;
		r->size--;
		return;
	}
//daca nu e indeplinita nicio conditie de mai sus, se efac legaturile
	m->next->prev = m->prev;
	m->prev->next = m->next;
	r->size--;

}

void add_in_truck(Truck* t, Package* p){
//functia adauga un pachet in camion

	Manifest* node = create_manifest_node();
	node->package = p;
	node->next = t->manifest;
	t->manifest = node;
	t->size++;

}

void robot_unload_packages(Truck* truck, Robot* robot){
	// TODO: 3.3
	if(truck == NULL || robot == NULL || robot_is_empty(robot)) return;

	Package* p = robot_get_destination_highest_priority_package(robot,truck->destination);

	if(p == NULL) return;

//se folosesc functiile de mai sus pentru a face transferul
	while(p && !truck_is_full(truck)){
		delete_package(p,robot);
		add_in_truck(truck,p);
		p = robot_get_destination_highest_priority_package(robot,truck->destination);
	}
	
	

}

// Attach to specific truck
int robot_attach_find_truck(Robot* robot, Parkinglot *parkinglot){
	int found_truck = 0;
	long size = 0;
	Truck *arrived_iterator = parkinglot->arrived_trucks->next;
	Manifest* m_iterator = robot->manifest;


	while(m_iterator != NULL){
		while(arrived_iterator != parkinglot->arrived_trucks){
			size  = truck_destination_robots_unloading_size(arrived_iterator);
			if(strncmp(m_iterator->package->destination, arrived_iterator->destination, MAX_DESTINATION_NAME_LEN) == 0 &&
					size < (arrived_iterator->capacity-arrived_iterator->size)){
				found_truck = 1;
				break;
			}

			arrived_iterator = arrived_iterator->next;
		}

		if(found_truck)
			break;
		m_iterator = m_iterator->next;
	}

	if(found_truck == 0)
		return 0;


	Robot* prevr_iterator = NULL;
	Robot* r_iterator = arrived_iterator->unloading_robots;
	while(r_iterator != NULL){
		Package *pkg = robot_get_destination_highest_priority_package(r_iterator, m_iterator->package->destination);
		if(m_iterator->package->priority >= pkg->priority)
			break;
		prevr_iterator = r_iterator;
		r_iterator = r_iterator->next;
	}

	robot->next = r_iterator;
	if(prevr_iterator == NULL)
		arrived_iterator->unloading_robots = robot;
	else
		prevr_iterator->next = robot;

	return 1;
}

void robot_print_manifest_info(Robot* robot){
	Manifest *iterator = robot->manifest;
	while(iterator != NULL){
		printf(" R->P: %s %ld\n", iterator->package->destination, iterator->package->priority);
		iterator = iterator->next;
	}

	printf("\n");
}



Truck* create_truck(const char* destination, long capacity, long transit_time, long departure_time){
	// TODO: 3.3

	Truck* truck = (Truck*)malloc(sizeof(Truck));
	truck->manifest = NULL;
	truck->unloading_robots = NULL;

	if(destination != NULL)
		truck->destination = strdup(destination);
	truck->size = 0;
	truck->capacity = capacity;
	truck->in_transit_time = 0;
	truck->transit_end_time= transit_time;
	truck->departure_time = departure_time;

	return truck; 
}

int truck_is_full(Truck *truck){
	// TODO: 3.3

	if(truck != NULL && truck->size == truck->capacity)
		return 1;
	 return 0;
}

int truck_is_empty(Truck *truck){
	// TODO: 3.3

	if(truck != NULL && truck->size == 0)
		return 1;
	 return 0;
}

long truck_destination_robots_unloading_size(Truck* truck){
	// TODO: 3.3

	if(truck == NULL || truck->unloading_robots == NULL || truck->destination == NULL) return 0;

	Manifest* m = truck->unloading_robots->manifest;
	Robot* r = truck->unloading_robots;
	long nr_package = 0,gasit = 0;

//se parcurge lista robotilor
	while(r->next != NULL){ 
		gasit = 0;
//se parcurge lista de pachete a fiecarui robot si se verifica conditia
		while(m->next != NULL && gasit != 1){
			if(strcmp(truck->destination,m->package->destination) == 0){
				nr_package = nr_package + r->size;
				gasit = 1; 
			}

			m = m->next;
		}

		r = r->next;
		m = r->manifest;
	}

	return nr_package;
}


void truck_print_info(Truck* truck){
	printf("T: %s %ld %ld %ld %ld %ld\n", truck->destination, truck->size, truck->capacity,
			truck->in_transit_time, truck->transit_end_time, truck->departure_time);

	Manifest* m_iterator = truck->manifest;
	while(m_iterator != NULL){
		printf(" T->P: %s %ld\n", m_iterator->package->destination, m_iterator->package->priority);
		m_iterator = m_iterator->next;
	}

	Robot* r_iterator = truck->unloading_robots;
	while(r_iterator != NULL){
		printf(" T->R: %ld %ld\n", r_iterator->size, r_iterator->capacity);
		robot_print_manifest_info(r_iterator);
		r_iterator = r_iterator->next;
	}
}


void destroy_truck(Truck* truck){
	// TODO: 3.3

	Robot* r = truck->unloading_robots;

	while(r != NULL){
		free(r->manifest);
		r = r->next;
	}

	free(truck->manifest);
	free(truck);

}


Parkinglot* create_parkinglot(void){
	// TODO: 3.4
	//Allocate parking lot

	Parkinglot* parkinglot = (Parkinglot*)malloc(sizeof(Parkinglot));
	parkinglot->arrived_trucks = (Truck*)malloc(sizeof(Truck));
	parkinglot->departed_trucks = (Truck*)malloc(sizeof(Truck));
	parkinglot->pending_robots = (Robot*)malloc(sizeof(Robot));
	parkinglot->standby_robots = (Robot*)malloc(sizeof(Robot));

	parkinglot->arrived_trucks->next = parkinglot->arrived_trucks;
	parkinglot->departed_trucks->next = parkinglot->departed_trucks;
	parkinglot->pending_robots->next = parkinglot->pending_robots;
	parkinglot->standby_robots->next = parkinglot->standby_robots;

	return parkinglot;
}

Parkinglot* open_parckinglot(const char* file_path){
	ssize_t read_size;
	char* line = NULL;
	size_t len = 0;
	char* token = NULL;
	Parkinglot *parkinglot = create_parkinglot();

	FILE *fp = fopen(file_path, "r");
	if(fp == NULL)
		goto file_open_exception;

	while((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		// destination, capacitym transit_time, departure_time, arrived
		if(token[0] == 'T'){
			token = strtok(NULL, ",\n ");
			char *destination = token;

			token = strtok(NULL, ",\n ");
			long capacity = atol(token);

			token = strtok(NULL, ",\n ");
			long transit_time = atol(token);

			token = strtok(NULL, ",\n ");
			long departure_time = atol(token);

			token = strtok(NULL, ",\n ");
			int arrived = atoi(token);

			Truck *truck = create_truck(destination, capacity, transit_time, departure_time);

			if(arrived)
				truck_arrived(parkinglot, truck);
			else
				truck_departed(parkinglot, truck);

		}else if(token[0] == 'R'){
			token = strtok(NULL, ",\n ");
			long capacity = atol(token);

			Robot *robot = create_robot(capacity);
			parkinglot_add_robot(parkinglot, robot);

		}

		free(line);
		line = NULL;
		len = 0;
	}
	free(line);

	fclose(fp);
	return parkinglot;

	file_open_exception:
	return NULL;
}

void parkinglot_add_robot(Parkinglot* parkinglot, Robot *robot){
	// TODO: 3.4

	Robot* r = parkinglot->standby_robots;
	Robot* r2 = parkinglot->pending_robots;

	if(robot_is_empty(robot)){
		while(1){
			if(r->next->capacity < robot->capacity){
				robot->next = r->next;
				r->next = robot;
				return;
			}
			
			r = r->next;
		}

	}else{
		while(1){
			if(r2->next->size < robot->size){
				robot->next = r2->next;
				r2->next = robot;
				return;
			}
			
			r2= r2->next;
	
		}
	}


}

void parkinglot_remove_robot(Parkinglot *parkinglot, Robot* robot){
	// TODO: 3.4

	if(parkinglot == NULL || robot == NULL) return;

	Robot* aux;

//se elimina robotul in functie de conditie
	if(!robot_is_empty(robot)){
		
		Robot* r = parkinglot->pending_robots;

		while(r->next != parkinglot->pending_robots){
			if(r->next == robot){
				aux = r->next;
				destroy_robot(aux);
				r->next = r->next->next;
				return;
			}

			r = r->next;
		}
	}else{

		Robot* r2 = parkinglot->standby_robots;

		while(r2->next != parkinglot->standby_robots){
			if(r2->next == robot){
				aux = r2->next;
				destroy_robot(aux);
				r2->next = r2->next->next;
				return;
			}

			r2 = r2->next;
		}
	}

}

int parckinglot_are_robots_peding(Parkinglot* parkinglot){
	// TODO: 3.4

	if(parkinglot->pending_robots->next == parkinglot->pending_robots)
		return 0;
	return 1;
}

int parkinglot_are_arrived_trucks_empty(Parkinglot* parkinglot){
	// TODO: 3.4

	if(parkinglot == NULL) return 0;

	Truck* t = parkinglot->arrived_trucks->next;


	while(t != parkinglot->arrived_trucks){
		if(!truck_is_empty(t))
			return 0;

		t = t->next;
	}

	return 1;
}


int parkinglot_are_trucks_in_transit(Parkinglot* parkinglot){
	// TODO: 3.4

	if(parkinglot->departed_trucks == parkinglot->departed_trucks->next)
		return 0;
	return 1;
}


void destroy_parkinglot(Parkinglot* parkinglot){
	// TODO: 3.4

	Truck* t = parkinglot->arrived_trucks->next;
	Truck* aux;


	while(t  != parkinglot->arrived_trucks){ //dealoc lisa arrived_trucks
		aux = t;
		free(aux);
		t = t->next;
	}
	free(parkinglot->arrived_trucks);


	Truck* t2 = parkinglot->departed_trucks->next;

	while(t2  != parkinglot->departed_trucks){ //dealoc lisa departed_trucks
		aux = t2;
		free(aux);
		t2 = t2->next;
	}
	free(parkinglot->departed_trucks);


	Robot* r = parkinglot->pending_robots->next;
	Robot* aux2;

	while(r  != parkinglot->pending_robots){//dealoc lisa pending_robots
		aux2 = r;
		free(aux2);
		r = r->next;
	}
	free(parkinglot->pending_robots);


	Robot* r2 = parkinglot->standby_robots->next;

	while(r2  != parkinglot->standby_robots){//dealoc lisa standby_robots
		aux2 = r2;
		free(aux2);
		r2 = r2->next;
	}
	free(parkinglot->standby_robots);

	free(parkinglot);

}

void parkinglot_print_arrived_trucks(Parkinglot* parkinglot){
	Truck *iterator = parkinglot->arrived_trucks->next;
	while(iterator != parkinglot->arrived_trucks){

		truck_print_info(iterator);
		iterator = iterator->next;
	}

	printf("\n");

}

void parkinglot_print_departed_trucks(Parkinglot* parkinglot){
	Truck *iterator = parkinglot->departed_trucks->next;
	while(iterator != parkinglot->departed_trucks){
		truck_print_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}

void parkinglot_print_pending_robots(Parkinglot* parkinglot){
	Robot *iterator = parkinglot->pending_robots->next;
	while(iterator != parkinglot->pending_robots){
		printf("R: %ld %ld\n", iterator->size, iterator->capacity);
		robot_print_manifest_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}

void parkinglot_print_standby_robots(Parkinglot* parkinglot){
	Robot *iterator = parkinglot->standby_robots->next;
	while(iterator != parkinglot->standby_robots){
		printf("R: %ld %ld\n", iterator->size, iterator->capacity);
		robot_print_manifest_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}


void truck_departed(Parkinglot *parkinglot, Truck* truck){
	// TODO: 3.5
	// Search through arrived list, if exists node is found remove it
	// Note: this must remove the node from the list, NOT deallocate it

	if(parkinglot == NULL || truck == NULL) return;

	Truck* t = parkinglot->arrived_trucks;
	Truck* t2 = parkinglot->departed_trucks;
	int gasit = 0;

	if(t->next == truck){ // daca primul element e chiar truck-ul,se elimina
		t->next = t->next->next;
		gasit = 1;
	}

	if(gasit == 0){ //se face eliminarea din arrived_trucks, daca exista
		while(t->next != parkinglot->arrived_trucks && gasit != 1){
			if(t->next == truck){
				t->next = t->next->next;
				gasit = 1;
			}	

			t = t->next;
		}
	}

	if(t2 == t2->next){ // daca nu exista niciun camion in departed_trucks,se adauga
		t2->next = truck;
		truck->next = t2;
		return;
	}

	while(t2->next != parkinglot->departed_trucks){//se adauga truck ul in lista
		if(truck->departure_time <= t2->next->departure_time){
			truck->next = t2->next;
			t2->next = truck;
			return;
		}

		t2 = t2->next;
	}

	//daca nu se gaseste niciun truck cu departure_time >= departure_time -ul
	// truck-ului primit ca parametru, se adauga la sfarsit

	truck->next = t2->next;
	t2->next = truck;
	return;
}


void truck_arrived(Parkinglot *parkinglot, Truck* truck){
	// TODO: 3.5
	// Search through departed list, if exists node is found remove it
	// Note: this must remove the node not deallocate it

	if(parkinglot == NULL || truck == NULL) return;

	Truck* t = parkinglot->arrived_trucks;
	Truck* t2 = parkinglot->departed_trucks;
	int gasit = 0;
	

	truck->size = 0;
	truck->manifest = NULL;

	if(t2->next == truck){ // daca primul element e chiar truck-ul,se elimina
		t2->next = t2->next->next;
		gasit = 1;
		truck->size = 0;
		truck->manifest = NULL;
	}

	if(gasit == 0){ //se face eliminarea din departed_truck, daca exista
		while(t2->next != parkinglot->departed_trucks && gasit != 1){
			if(t2->next == truck){
				t2->next = t2->next->next;
				gasit = 1;
				truck->size = 0;
				truck->manifest = NULL;
			}	

			t2 = t2->next;
		}
	}

	if(t == t->next){ // daca nu exista niciun camion in arrived_trucks, se adauga
		t->next = truck;
		truck->next = t;
		return;
	}

//se face adaugarea in functie de sortarea ceruta in cerinta
	while(t->next != parkinglot->arrived_trucks){
		if(strcmp(truck->destination,t->next->destination) < 0){
			truck->next = t->next;
			t->next = truck;
			return;
		}

		if(strcmp(truck->destination,t->next->destination) == 0){
			while(t->next != parkinglot->arrived_trucks && strcmp(truck->destination,t->next->destination) == 0){
				if(truck->departure_time < t->next->departure_time){
					truck->next = t->next;
					t->next = truck;
					return;
				}

				t = t->next;
			}

			truck->next = t->next;
			t->next = truck;
			return;
		}
				

		t = t->next;
	}

	truck->next = t->next;
	t->next = truck;
	return;


}

void truck_transfer_unloading_robots(Parkinglot* parkinglot, Truck* truck){
	// TODO:  3.5

	Robot* r = truck->unloading_robots;
	Robot* r2;
//se face transferul, iterand lista de roboti
	while(r != NULL){
		r2 = r;
		r = r->next;
		parkinglot_add_robot(parkinglot,r2);
	}

	truck->unloading_robots = NULL;

}


// Depends on parking_turck_departed
void truck_update_depatures(Parkinglot* parkinglot, long day_hour){
	// TODO: 3.5

	Truck* t = parkinglot->arrived_trucks;
	Truck* t2;
//se parcurge lista, se verifica departure_time -ul si se face update
	while(t->next != parkinglot->arrived_trucks){
		t2 = t->next;

		if(t2->departure_time == day_hour){
			truck_transfer_unloading_robots(parkinglot,t2);
			truck_departed(parkinglot,t2);
		}

		t = t->next;
	}
}

// Depends on parking_turck_arrived
void truck_update_transit_times(Parkinglot* parkinglot){
	// TODO: 3.5

	Truck* t = parkinglot->departed_trucks;
	Truck* t2;
//se parcurge lista, se verifica transit_time -ul si se face update
	while(t->next != parkinglot->departed_trucks){
		t2 = t->next;
		t2->in_transit_time++;

		if(t2->in_transit_time == t2->transit_end_time){
			t2->in_transit_time = 0;
			truck_arrived(parkinglot, t2);
		}

		t = t->next;
	}
		
	
}

void robot_swarm_collect(Wearhouse *wearhouse, Parkinglot *parkinglot){
	Robot *head_robot = parkinglot->standby_robots;
	Robot *current_robot = parkinglot->standby_robots->next;
	while(current_robot != parkinglot->standby_robots){

		// Load packages from wearhouse if possible
		if(!robot_load_packages(wearhouse, current_robot)){
			break;
		}

		// Remove robot from standby list
		Robot *aux = current_robot;
		head_robot->next = current_robot->next;
		current_robot = current_robot->next;

		// Add robot to the
		parkinglot_add_robot(parkinglot, aux);
	}
}


void robot_swarm_assign_to_trucks(Parkinglot *parkinglot){

	Robot *current_robot = parkinglot->pending_robots->next;

	while(current_robot != parkinglot->pending_robots){
		Robot* aux = current_robot;
		current_robot = current_robot->next;
		parkinglot_remove_robot(parkinglot, aux);
		int attach_succeded = robot_attach_find_truck(aux, parkinglot);
		if(!attach_succeded)
			parkinglot_add_robot(parkinglot, aux);
	}
}

void robot_swarm_deposit(Parkinglot* parkinglot){
	Truck *arrived_iterator = parkinglot->arrived_trucks->next;
	while(arrived_iterator != parkinglot->arrived_trucks){
		Robot *current_robot = arrived_iterator->unloading_robots;
		while(current_robot != NULL){
			robot_unload_packages(arrived_iterator, current_robot);
			Robot *aux = current_robot;
			current_robot = current_robot->next;
			arrived_iterator->unloading_robots = current_robot;
			parkinglot_add_robot(parkinglot, aux);
		}
		arrived_iterator = arrived_iterator->next;
	}
}

