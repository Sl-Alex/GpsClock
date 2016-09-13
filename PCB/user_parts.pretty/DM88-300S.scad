p_size = 0.3; // width and depth

module dots(){
	for ( x = [0 : 7] )
	{
		for ( y = [0 : 7] )
		{
			translate([x * 4 + 2, y * 4 + 2, 6.9])
			cylinder(h=0.2, r = 1.5, center= false, $fn = 10);
		}
	}
}

translate([-2,-4,0.01])
{
	case();
	pins_array();
}

module case_cutout(){
	union() {
		translate([0, 33, 0])
			rotate([90, 0, 0])
				linear_extrude(height = 34, center = false, convexity = 10)
					polygon([[1,-1],[1,0],[2,1],[30,1],[31,0],[31,-1]]);

		translate([+33, +32, 0])
			rotate([90, 0, 270])
				linear_extrude(height = 34, center = false, convexity = 10)
					polygon([[1,-1],[1,0],[2,1],[30,1],[31,0],[31,-1]]);
		translate([0.5, 0.5, -1])
			cube([32 - 1, 32 - 1, 2]);
	}
}

module case(){
	difference() {
		difference() {
			cube([32,32,7]);
			case_cutout();
		}
		dots();
	}
}

module pins(){
	for ( i = [0 : 7] )
	{
		translate([-p_size/2 + i * 2.54, - p_size/2 + 4, -2.5])
			cube([p_size, p_size, 4]);
	}
}

module pins_array(){
	translate([(32 - 7*2.54)/2, 0, 0])
		pins();
	translate([(32 - 7*2.54)/2, 24, 0])
		pins();
}
