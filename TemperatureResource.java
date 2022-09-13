package app.resources;

import java.util.List;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.QueryParam;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.Response.Status;

import app.models.Temperature;
import app.repository.TemperatureRepository;


@Path("temperatures")
public class TemperatureResource {
	
	TemperatureRepository repository;
	
	
	public TemperatureResource() {
		repository = new TemperatureRepository();
	}
	
	@GET
	@Produces(MediaType.APPLICATION_JSON)
	public Response listaAll(@QueryParam("ini") String ini, @QueryParam("end") String end) {
		
		List<Temperature> temperatures = null;
		
		if(ini != null && end != null) {
			temperatures = repository.getByTimeInterval(ini, end);
		} else {
			temperatures = repository.findAll();
		}
		
		return Response.status(Status.OK).entity(temperatures).build();
				
	}
	
	public void findById(){
		
	}

	
}
