int a;
main()
{
	int b;
	int a;
	int i;
	do {
		b = (a && 1);
		if (a == 1) {
			b = 3;
			for (i = 0 ; i < 5 ; i = 3) {
				b = (b || (i < 3));
				a = (a < b) ? 2 : 3;
			}
		}
	} while (1 < a);
	return;
}
