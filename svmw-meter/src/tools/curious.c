#include <stdio.h>

#include <time.h>

void untime(unsigned int t) {

	int seconds;
	int minutes;
	int hours;
	int days;
	int years;

	if (t<60) {
		printf("%d seconds\n",t);
		return;
	}

	minutes=t/60;
	seconds=t-(minutes*60);

	if (minutes<60) {
		printf("%d minutes %d seconds\n",minutes,seconds);
		return;
	}

	hours=minutes/60;
	minutes=minutes-(hours*60);

	if (hours<24) {
		printf("%d hours %d minutes %d seconds\n",
			hours,minutes,seconds);
		return;
	}

	days=hours/24;
	hours=hours-(days*24);
	if (days<365) {
		printf("%d days %d hours %d minutes %d seconds\n",
			days,hours,minutes,seconds);
		return;
	}

	years=days/365;
	days=days-(years*365);

	printf("%d years %d days %d hours %d minutes %d seconds\n",
		years,days,hours,minutes,seconds);

};

void nextflip(unsigned long long t, time_t current_time) {

	time_t new_time;

	new_time=current_time+t;

	printf("%s",ctime(&new_time));

}

void flip_to_one(unsigned long long t, time_t current_time) {

	time_t new_time;

	if ((current_time&t)==t) printf("Now\n");
	else {
		new_time=current_time|t;
		new_time&=~(t-1);
		printf("%llx %s",(unsigned long long)new_time,ctime(&new_time));
	}
}

void ones_in_row(unsigned long long t, time_t current_time) {

	time_t new_time;

	if ((current_time&t)==t) printf("Now\n");
	else {
		new_time=current_time|t;
		printf("%llx %s",(unsigned long long)new_time,ctime(&new_time));
	}
}


int main(int argc, char **argv) {

	unsigned long long i;
	time_t current_time;



	for(i=0;i<32;i++) {
		printf("%lld: ",i);
		untime(1UL<<i);
	}

	printf("\n");
	printf("Current time:\n");
	current_time=time(NULL);
	printf("%s",ctime(&current_time));

	for(i=0;i<32;i++) {
		printf("%lld: ",i);
		nextflip(1ULL<<i,current_time);
	}

	for(i=0;i<32;i++) {
		printf("%lld: ",i);
		flip_to_one(1ULL<<i,current_time);
	}

	printf("String of ones\n");
	for(i=0;i<32;i++) {
		printf("%lld: ",i);
		ones_in_row((1ULL<<i)-1,current_time);
	}



	return 0;
}
