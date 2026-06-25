iterations =8;
height = 60;
trunk_smoothness = 4;


bottom_width = 0.25;
top_width = 0.20;
knuckle_size = 0.25;
rate_of_decay = 0.8;
leaf_smoothness = 10;
leaf_size = 1.5;


module trunk(size, depth){
    //create an array of random numbers
    operation = rands(0,4,1);

    if (size > 5){
        //split into two branches
        if (operation[0] < 2){
            branch_two(size*.9, depth);          
        }
        //three branches
        else if (operation[0] < 3){
            branch_three(size*.9, depth);          
        }
        //4 branches
        else if (operation[0] < 4){
            branch_four(size*.9, depth);          
        }      
    }  
}



module branch_two(size, depth){
    sizemod = rands(rate_of_decay,1.15,10);
	entropy = rands(0.5,leaf_size,1)[0];
	rotations = rands(-20,20,10);

	color([0.3,0.15,0]){
		cylinder(h = size, r1 = size*bottom_width, r2 = size*top_width);
	}

	translate([0,0,size]){
		if (depth > 0){
            union(){
                color([0.3,0.15,0]){
                    sphere(size*knuckle_size);
                    echo("knuckle2");
                }    
                rotate([0+rotations[0],30+rotations[1],0+rotations[2]]){
                    trunk(size*.9*sizemod[0], depth-1);
                }
                rotate([0+rotations[3],30+rotations[4],180+rotations[5]]){
                    //recursivly call trunk
                    trunk(size*.9*sizemod[1], depth-1);
                }
            }   
		}
		else{
			color([0,0.6,0]){
				sphere(size*.5*entropy, $fn=leaf_smoothness);
			}
		}
	}
}

module branch_three(size, depth){
    sizemod = rands(rate_of_decay,1.15,10);
	entropy = rands(0.5,leaf_size,1)[0];
	rotations = rands(-20,20,10);

	color([0.3,0.15,0]){
		cylinder(h = size, r1 = size*bottom_width, r2 = size*top_width);
	}

	translate([0,0,size])
	{
		
		if (depth > 0) 
		{
            union() 
            {
                color([0.3,0.15,0]){
                    sphere(size*knuckle_size);
                    echo("knuckle3");
                }
                rotate([0+rotations[0],30+rotations[1],0+rotations[2]])
                {
                    //recursivly call trunk
                    trunk(size*.9*sizemod[0], depth-1);
                }
                rotate([0+rotations[3],30+rotations[4],120+rotations[5]])
                {
                    //recursivly call trunk
                    trunk(size*.9*sizemod[1], depth-1);
                }
                rotate([0+rotations[6],30+rotations[7],240+rotations[8]])
                {
                    //recursivly call trunk
                    trunk(size*.9*sizemod[2], depth-1);
                }
            }   
		}
		else
		{
			color([0,0.8,0]) {
				sphere(size*.5*entropy, $fn=leaf_smoothness);
			}
		}
	}
}

module branch_four(size, depth){
    sizemod = rands(rate_of_decay,1.15,10);    
	entropy = rands(0.5,leaf_size,1)[0];
	rotations = rands(-20,20,12);

	color([0.3,0.15,0]){
		cylinder(h = size, r1 = size*bottom_width, r2 = size*top_width);
	}


	translate([0,0,size]){	
		if (depth > 0){
            union(){
                color([0.3,0.15,0]){
                    sphere(size*knuckle_size);
                    echo("knuckle4");
                }
                rotate([0+rotations[0],30+rotations[1],0+rotations[2]]){
                //recursivly call trunk
                    trunk(size*.9*sizemod[0], depth-1);
                }
                rotate([0+rotations[3],30+rotations[4],90+rotations[5]]){
                //recursivly call trunk
                    trunk(size*.9*sizemod[1], depth-1);
                }
                rotate([0+rotations[6],30+rotations[7],180+rotations[8]]){
                //recursivly call trunk
                    trunk(size*.9*sizemod[2], depth-1);
                }
                rotate([0+rotations[9],30+rotations[10],270+rotations[11]]){
                //recursivly call trunk
                    trunk(size*.9*sizemod[2], depth-1);
                }                
            }
		}
		else{
			color([0,1,0]) {
				sphere(size*.5*entropy, $fn=leaf_smoothness);
			}
		}
	}

}


trunk(height, iterations, $fn=trunk_smoothness);



